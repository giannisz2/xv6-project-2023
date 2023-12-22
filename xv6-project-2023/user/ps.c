#include "user.h"
#include "syscall.h"
#include "kernel/proc.h"
#include "param.h"


int main(void) {
    struct proc* proc;
    struct pstat* pinfo;
    
    for(struct proc* i = proc; i < &proc[NPROC]; i++) { // For every possible process running on the system
        if(getpinfo(pinfo) == -1) { // Error checking
            printf("Failure.\n");
            return -1;
        }
        if(pinfo->state != RUNNING) // If process isn't active
            continue; 
            
        printf("The info for process %d are \n Name: %s \n PID: %d \n PPID: %d \n Priority %d \n State %d \n Length: %ld ", 
        i, pinfo->name, pinfo->pid, pinfo->ppid, pinfo->priority, pinfo->state, pinfo->length);
    }
    
    return 0;
}