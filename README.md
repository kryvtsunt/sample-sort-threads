Sample Sort
In this assignment, you'll rewrite your sample sort program to use:

Threads instead of processes
Mutexes/cond vars instead of semaphores
Read/write/lseek syscalls instead of mmap for IO.
Don't sort in place, write the results to an output file.
See assignment.txt in the download for the updated algorithm description.

This time, a barrier primitive is not provided. Write your own barrier primitive using mutexes and cond vars. The general idea of a barrier goes as follows:

A barrier is initialized with N, the number of threads that need to reach the barrier before any can continue.
It maintains a count of the number of processes that have reached the barrier.
The first N-1 threads to reach the barrier block.
Once the N'th thread reaches the barrier, all the threads are allowed to continue.
It's OK if your barrier can't be reused.
Each worker thread should print out some text, showing that it's actually participating in sample sort:

 {p}: start {start}, count {length of local array}  
This should be the only output of your finished program.

Homework Tasks
For this assignment you should avoid using the CCIS Linux server. Work on your local machine, a local Linux VM or container, or use a lab machine in WVH.

This is a good opportunity to install a local Linux VM. I suggest VirtualBox and Xubuntu 16.04. You'll need local Linux for HW09 and CH03 anyway.

Write a your program.
Collect execution times for 1, 4, and 8 threads.
Generate a graph.png and write a report.txt
Task 2: Measure Parallel Speedup
Time your tests using the "/usr/bin/time -p" command. (You're interested in "real" time)
Repeat each test three times, taking the median.
Make a graph (graph.png) showing the measured time for each case.
Task 3: report.txt
Create a file, report.txt, containing the following information:

The operating system and number of processor cores of your test machine (which should not be login.ccs.neu.edu).
A table of measured time and parallel speedup for each test.
Parallel speedup is calculated as (time for 1 thread / time for N threads)
Optimal parallel speedup is N.
A paragraph or two discussing what results you got and why you got them.
If you're testing on a different machine than for HW05, rerun your HW05 tests on the new machine for comparison.
How do you results compare to the process / mmap version? Why?
