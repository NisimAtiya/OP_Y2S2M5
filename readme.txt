Instructions for Running the Assignment
This assignment involves a multithreaded pipeline implementation in C. It consists of multiple active objects (AOs) that communicate through queues to process tasks concurrently. The tasks involve generating random numbers, performing prime number checks, and manipulating numbers.

Prerequisites
To run the assignment, ensure that you have the following installed on your system:

C compiler (e.g., GCC)
POSIX threads (pthread) library
Compilation
Open a terminal or command prompt.

Navigate to the directory where you have the source code file (st_pipeline.c) saved.
Compile the code using the C compiler. heer  we'll use GCC.

Copy code
make all
This command compiles the code and generates an executable named st_pipeline.

Running the Program
Once the code is compiled successfully, follow these steps to run the program:

In the terminal or command prompt, execute the compiled program.

Copy code
./st_pipeline <N> [RAND]
Replace <N> with the desired number of tasks to be processed by the pipeline. The optional [RAND] argument can be used to specify a random seed for generating random numbers. If not provided, a random seed will be generated internally.

Example 1: Process 100 tasks with a random seed

Copy code
./st_pipeline 100
Example 2: Process 50 tasks with a specific random seed

bash
Copy code
./st_pipeline 50 12345
The program will start executing the tasks in the pipeline. The output will be displayed on the console, showing the generated numbers, their primality, and any modifications made to the numbers.

Once all tasks are completed, the program will terminate.

Exiting the Program
You can exit the program by pressing Ctrl + C in the terminal or command prompt.
