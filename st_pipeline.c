#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>

// Part A: Check if a number is prime

// Function to check if a number is prime
int isPrime(unsigned int num)
{
    if (num == 2)
    {
        printf("%d\ntrue\n",num);
        return 1;
    }

    for (int i = 2; i * i <= num; i++)
    {
        if ((num % i) == 0)
        {
            printf("%d\nfalse\n",num);
            return 0;
        }
    }
    printf("%d\ntrue\n",num);
    return 1;
}

// Part B: Queue implementation

typedef struct Node
{
    void* task;
    struct Node* next;
} Node, *Pnode;

typedef struct Queue
{
    Pnode head;
    int size;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} Queue, *Pqueue;

// Create a new node
Pnode new_node(void* task)
{
    Pnode newnode = (Pnode)malloc(sizeof(Node));
    if (newnode == NULL)
    {
        perror("new_node");
        exit(-1);
    }
    newnode->task = task;
    newnode->next = NULL;
    return newnode;
}

// Initialize a new queue
Pqueue initializeQueue()
{
    Pqueue new_queue = (Pqueue)malloc(sizeof(Queue));
    if (new_queue == NULL)
    {
        perror("initializeQueue");
        exit(-1);
    }
    new_queue->head = NULL;
    new_queue->size = 0;
    pthread_mutex_init(&new_queue->mutex, NULL);
    pthread_cond_init(&new_queue->cond, NULL);

    return new_queue;
}

// Enqueue a task
void enqueue(Pqueue queue, void* task)
{
    pthread_mutex_lock(&queue->mutex);
    Pnode new_task = new_node(task);

    new_task->next = queue->head;
    queue->head = new_task;
    queue->size++;

    pthread_mutex_unlock(&queue->mutex);
    pthread_cond_signal(&queue->cond);
}

// Dequeue a task
void* dequeue(Pqueue queue)
{
    pthread_mutex_lock(&queue->mutex);
    while (queue->size <= 0)
    {
        pthread_cond_wait(&queue->cond, &queue->mutex);
    }

    Pnode current = queue->head;
    Pnode previous = NULL;
    while (current->next != NULL)
    {
        previous = current;
        current = current->next;
    }

    void* task = current->task;
    if (previous != NULL)
    {
        previous->next = NULL;
    }
    else
    {
        queue->head = NULL;
    }
    queue->size--;

    free(current);
    pthread_mutex_unlock(&queue->mutex);
    return task;
}

// Remove the queue and free memory
void removeQueue(Pqueue queue)
{
    Pnode current = queue->head;
    while (current != NULL)
    {
        Pnode next = current->next;
        free(current);
        current = next;
    }
    free(queue);
}

// Part C: Active Object (AO) implementation

typedef struct AO
{
    Pqueue queue;
    void (*func)(struct AO*, void*);
    pthread_t thread;
    struct AO* next;
} AO, *PAO;

// Thread function for AO
static void* aoThread(void* arg)
{
    PAO ao = (PAO)arg;
    void* task;
    while ((task = dequeue(ao->queue)) != NULL)
    {
        ao->func(ao->next, task);
    }
    return task;
}

// Create a new Active Object
PAO CreateActiveObject(PAO next, void (*func)(PAO, void*))
{
    PAO ao = (PAO)malloc(sizeof(AO));
    if (ao == NULL)
    {
        perror("CreateActiveObject");
        exit(-1);
    }

    ao->queue = initializeQueue();
    ao->next = next;
    ao->func = func;
    pthread_create(&ao->thread, NULL, aoThread, ao);
    return ao;
}

// Get the queue associated with an AO
Pqueue getQueue(PAO ao)
{
    return ao->queue;
}

// Stop an AO and clean up resources
void stop(AO* ao)
{
    pthread_cancel(ao->thread);
    removeQueue(ao->queue);
    free(ao);
}

// Part D: Functions to be executed by AOs

// Function 1: Generate a random number, enqueue it, and pass it to the next AO
void func1(PAO next, void* rand_seed)
{
    int num = *(int*)rand_seed;
    srand(num);
    int min = 100000;
    int max = 999999;
    int rand_num = (rand() % (max - min + 1)) + min;
    void* task = &rand_num;
    usleep(1000);
    enqueue(getQueue(next), task);
}

// Function 2: Check if a number is prime, modify it, and pass it to the next AO
void func2(PAO next, void* task)
{
    int num = *(int*)task;
    isPrime(num);
    num += 11;
    void* task2 = &num;
    enqueue(getQueue(next), task2);
}

// Function 3: Check if a number is prime, modify it, and pass it to the next AO
void func3(PAO next, void* task)
{
    int num = *(int*)task;
    isPrime(num);
    num -= 13;
    void* task3 = &num;
    enqueue(getQueue(next), task3);
}

// Function 4: Print a number and modify it
void func4(PAO next, void* task)
{
    int num = *(int*)task;
    printf("%d\n", num);
    num += 2;
    printf("%d\n", num);
}

int main(int argc, char* argv[])
{
    int rand_seed = 0;
    if (argc < 2 || argc > 3)
    {
        printf("usage: st_pipeline <N> [RAND] \n");
        return (1);
    }
    if (argc == 2)
    {
        srand(time(NULL));
        int min = 0;
        int max = 9;
        rand_seed = (rand() % (max - min + 1)) + min;
    }
    if (argc == 3)
    {
        rand_seed = atoi(argv[2]);
    }

    // Create the Active Objects (AOs) with their respective functions
    PAO fourthAO = CreateActiveObject(NULL, func4);
    PAO thirdAO = CreateActiveObject(fourthAO, func3);
    PAO secondAO = CreateActiveObject(thirdAO, func2);
    PAO firstAO = CreateActiveObject(secondAO, func1);

    void* task = &rand_seed;

    // Enqueue tasks to the first AO
    for (int i = 0; i < atoi(argv[1]); i++)
    {
        int rand_seed = *(int *)task;
        enqueue(getQueue(firstAO), &rand_seed);
    }

    while (firstAO->queue->size > 0)
    {
    }
    sleep(1);
    stop(firstAO);

    while (secondAO->queue->size > 0)
    {
    }
    stop(secondAO);

    while (thirdAO->queue->size > 0)
    {
    }
    stop(thirdAO);

    while (fourthAO->queue->size > 0)
    {
    }
    stop(fourthAO);
}