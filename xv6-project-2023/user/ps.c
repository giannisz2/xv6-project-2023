#include "user.h"
#include "syscall.h"
#include "kernel/proc.h"
#include "kernel/param.h"


int main(void) {
    struct proc* proc;
    struct pstat* pinfo = malloc(sizeof(struct pstat)); // initialize struct pstat
    if(getpinfo(pinfo) == -1) { // Error checking
            printf("Failure.\n");
            return -1; 
    }

    printf("Name\tPID\t\tPPID\tPriority\tState\tLength");
    for(int i = 0; i < NPROC; i++) {
        if(pinfo->pid[i] == 0)
            continue;
        printf("%s\t%d\t%d\t%d\t%d\t%ld", pinfo->name[i], pinfo->pid[i], pinfo->ppid[i], pinfo->priority[i], pinfo->state[i], pinfo->length[i]);
        printf("\n");
    }
    
    
    return 0;
}