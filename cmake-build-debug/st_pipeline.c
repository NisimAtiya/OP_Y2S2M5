#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <thread>

// Part A:

int isPrime(unsigned int num)
{
    if(num==2){
        return 1;
    }
    if ((num % 2) == 0)
        return 0;
    for (int i = 3; i * i <= num; i += 2)
    {
        if ((num % i) == 0)
            return 0;
    }
    return 1;
}



// Part B:

typedef struct Node
{
    void *task;
    struct Node *next;
} Node, *Pnode;

// Define the queue structure
typedef struct Queue
{
    Pnode head;
    int size;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} Queue, *Pqueue;

Pnode new_node(void *task)
{
    Pnode Newnode = (Pnode)malloc(sizeof(Node));
    if (Newnode == NULL)
    {
        perror("Pnode new");
        exit(-1);
    }
    new_node->task = task;
    new_node->next = NULL;
    return Newnode;
}

// Initialize the queue
Pqueue initializeQueue()
{
    Pqueue new_queue = (Pqueue)malloc(sizeof(Queue));
    if (new_queue == NULL)
    {
        perror("Pqueue new");
        exit(-1);
    }
    new_queue->head = NULL;
    new_queue->size = 0;
    pthread_mutex_init(&new_queue->mutex, NULL);
    pthread_cond_init(&new_queue->cond, NULL);

    return new_queue;
}

// Enqueue an task
void enqueue(Pqueue queue, void *task)
{
    pthread_mutex_lock(&queue->mutex);
    Pnode new_task = new_node(task);

    if (queue->size == 0)
    {
        queue->head = new_task;
        queue->size = 1;
    }
    else
    {
        new_task->next = queue->head;
        queue->head = new_task;
        queue->size++;
    }
    pthread_mutex_unlock(&queue->mutex);
    pthread_cond_signal(&queue->cond);
}

// Dequeue an task
void *dequeue(Pqueue queue)
{
    pthread_mutex_lock(&queue->mutex);
    while (queue->size <= 0)
    {
        pthread_cond_wait(&queue->cond, &queue->mutex);
    }
    Pnode current = queue->head;

    while (current->next != NULL)
    {
        if (current->next->next == NULL)
        {
            break;
        }
        current = current->next;
    }
    void *task = current->next->task;
    Pnode to_free = current->next;
    current->next = NULL;
    queue->size--;
    free(to_free);

    pthread_mutex_unlock(&queue->mutex);
    return task;
}

void removeQueue(Pqueue queue)
{
    Pnode current = queue->head;
    while (current != NULL)
    {
        Pnode to_free = current;
        current = current->next;
        free(to_free);
    }
    free(queue);
}

// Part C:

typedef int (*handler)(void*);

struct AO {
    std::thread _thread;
    handler _func;
    Queue* _queue;
    bool _is_alive;
};

void CreateActiveObject(struct AO* ao, struct Queue* queue, handler func) {
    ao->_queue = queue;
    ao->_func = func;
    ao->_is_alive = 1;

    pthread_create(&ao->_thread, NULL, activeObjectThread, ao);
}

void* activeObjectThread(void* arg) {
    struct AO* ao = (struct AO*)arg;

    while (ao->_is_alive) {
        void* task = dequeue(ao->_queue);
        if (task != NULL) {
            ao->_func(task);
        }
    }

    return NULL;
}

Queue* getQueue(AO* ao) {
    return ao->_queue;
}
void stop(struct AO* ao) {
    ao->_is_alive = 0;  // Set _is_alive flag to false to stop the active object

    if (pthread_join(ao->_thread, NULL) != 0) {
        perror("pthread_join");
        exit(-1);
    }

    removeQueue(ao->_queue);
    ao->_queue = NULL;

    free(ao);
}
// Pipeline initialization and processing function
void pipeline_st(int N, unsigned int seed) {
    // Initialize the random number generator
    srand(seed);

    // Create the queues and AOs
    Queue* queue1 = initializeQueue();
    Queue* queue2 = initializeQueue();
    Queue* queue3 = initializeQueue();
    Queue* queue4 = initializeQueue();

    struct AO ao1, ao2, ao3, ao4;
    CreateActiveObject(&ao1, queue1, NULL);
    CreateActiveObject(&ao2, queue2, NULL);
    CreateActiveObject(&ao3, queue3, NULL);
    CreateActiveObject(&ao4, queue4, NULL);

    // Generate N numbers
    for (int i = 0; i < N; i++) {
        // Generate a 6-digit number
        int number = (rand() % 900000) + 100000;

        // Enqueue the number into the first queue
        enqueue(queue1, (void*)(intptr_t)number);

        // Sleep for 1 millisecond
        usleep(1000);
    }

    // Stop the AOs
    stop(&ao1);
    stop(&ao2);
    stop(&ao3);
    stop(&ao4);

    // Cleanup
    removeQueue(queue1);
    removeQueue(queue2);
    removeQueue(queue3);
    removeQueue(queue4);
}

int main(int argc, char* argv[]) {
    if (argc < 2 || argc > 3) {
        printf("Usage: pipeline_st N [seed]\n");
        return 1;
    }

    int N = atoi(argv[1]);
    unsigned int seed;

    if (argc == 3) {
        seed = atoi(argv[2]);
    } else {
        seed = (unsigned int)time(NULL);
    }

    pipeline_st(N, seed);

    return 0;
}
