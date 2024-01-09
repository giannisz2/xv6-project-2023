#include "user.h"
#include "kernel/types.h"
#include "kernel/pstat.h"
#include "kernel/stat.h"


int main(void) {
    struct pstat* pstat = malloc(sizeof(struct pstat));

    if(getpinfo(pstat) < 0) {
        printf("Failure in ps file.\n");
        return -1;
    }
    printf("Name\tPID\t\tPPID\tPriority\tState\tLength");
    for(int i = 0; i < NPROC; i++) {
        if(pstat->pid[i] == 0) {
            continue;
        }
        printf("%s\t%d\t%d\t%d\t%d\t%d", pstat->name[i], pstat->pid[i], pstat->ppid[i], pstat->priority[i], pstat->state[i], pstat->length[i]);
    }
    return 0;
}