# operating-systems-xv6-emulator-project

First I added to proc.h the variable "int priority" in the struct proc where it is needed to return the priority of the specific process, as well as initializing it in allocproc() found in proc.c. The struct pstat containing various process data is defined in pstat.h and consists of NPROC sized tables that will contain all the data of all processes at once. I then added to syscall.h two new numbers corresponding to the two new system calls, to user.h the numbers for the calls, and to usys.pl the two calls as entries.

Then I implemented the two system calls in sysproc.c as follows: 
-In uint64 sys_priority(void): I set an int num variable to be used in argint() to get the priority argument. I then check if the priorities are within bounds and if they are not then it returns -1 unsuccessfully. Otherwise I get the struct of the process currently running with myproc() and after making aqcuire() lock it so that no other process from another cpu can write something there at the same time, I set the priority of the process to num. I then release() the lock and return 0 (success).
-In uint64 sys_getpinfo(void): first I set a uint64 pointer. Then I get the struct of the process currently running with myproc(), with argaddr() I get the pstat argument given by ps.c to the pointer and do kalloc() on a *pstat struct. I use copyout() for error checking, and then run all processes with a for loop. Inside the for loop I make acquire() lock so that no other process can write, check if the process is active or ready to run, copy all the data from the *p struct to the *pstat struct, and print the results. I then release() the lock and return 0 (success).

Then in the ps.c file I just call getpinfo() where it does all the work and I also added $U/_ps\ to the Makefile on line 136 so I can run ps on the emulator. 

The implementation of priority_scheduler() was implemented as follows: 
I start by defining the struct proc* p variable where I will use for the loop where all the processes run.
Then I take in the struct cpu* c variable the struct of the given cpu with mycpu() and initialize the cpu process to 0.
Inside while(1): 
-I enable the interrupts with intr_on().
-I initialize min to 21 since the process cannot have a lower priority than 20.
-I initialize the new process to run to 0.
-I start with the for loop to loop through all the processes that are ready to run at that moment and those already running in the case of large preempted tasks, and check if their priority is higher than the others. When the highest is found, it will run.
-If a process with higher priority is now found, then I set its state as RUNNING, put it on cpu, swtch() the context, and initialize it back to 0 because it stopped running.
-If it's not found, it goes to the next one.
If two processes have the same priority in essence they will be selected by a round-robin policy.

The usertests all come out OK except for killstatus (where it stuck there without showing anything), preempt (same as killstatus) and reparent (which returned fail) where I commented them out in usertests and they just return 0 so I can check the rest.
In priotest the processes seem to be executed in the correct order.
