# testtaskqueue
Demo project for midas interview task

## Getting Started
These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. See deployment for notes on how to deploy the project on a live system.

## Prerequisites
cmake - https://cmake.org/install/

## Installing
Example is using a bash console, where CWD is the testtaskqueue folder at the root of the project.
Open a console and;
cd CWD;
mkdir build;
cd build;
cmake ../;
make;

The output executable is named testtaskqueue and is located in the build folder, to run call
./testtaskqueue

## Running the tests
No automated tests are present, but the testtestqueue app allows the user to set test parameters to verify the operation.
The test starts a number of producer threads that contain messages to be printed to the console, each message contains the internal worker thread id starting from 0 and the job number for that thread starting from 0.

If a situation occurs where a producer thread cannot post a job to the task queue because it is full, the message will be skipped. As the consumer thread processes the jobs it prints the message to the console. At the end of the test any skipped messages will be printed. The test passes if the number of jobs processed and the skipped jobs are equal to the number of jobs that were created by the worker threads. This shows no messages were lost by the queue. The user may also check that the jobs printed by the consumer are displayed correctly and that the job numbers per thread always increase, although some jobs maybe missing, but must then be in the skipped jobs list.    

Some example parameter configurations for different tests are;

### General purpose test
Use the default parameters, enter the command: run

### Skipped jobs due to too much work for the consumer
Consumer thread time to execute a job (ms)
Jobs will be skipped but none should be lost.

### Small queue (buffer wrapping) test
Set the FIFO queue length to 2 and run
Many jobs will be skipped but none should be lost.

### Pop try test
This can be used in conjunction with each of the above tests, but the following example slows down job production to ensure no jobs are available at times so pop try fails to pop a job.
Set the Consumer gets jobs via pop_try (1).
Set the worker Producer thread time between producing jobs (ms) to 100.
run
Output should include the following statement, where X is the "Consumer thread time to execute a job (ms)" parameter value.
No job ready - poll again in X ms

## Built With
cmake v3.14.1 on Mac OS X
cmake v3.14.1 on Linux Cent OS 7

## Author
Luke Hollingworth

## License
License free

## TODO
Improve test pass/fail test conditions to check jobs are printed in the correct order for each thread and no jobs are missing.
Comment the MainMenu and TestTaskQueue classes.
Create and verify documentation using Doxygen.
Add output file selection and write the messages and test results to the output file.
Clear the command window each time it is updated, keeping the last test output below the menu.
Improve sanitisation the testtaskqueue command line inputs.
Add automated tests.


