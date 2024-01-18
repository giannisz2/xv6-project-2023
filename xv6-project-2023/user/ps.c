#include "user.h"
#include "kernel/spinlock.h"
#include "kernel/riscv.h"
#include "kernel/pstat.h"
#include "kernel/proc.h"

int main(void) {

  struct pstat* pstat = malloc(sizeof(struct pstat));  
  
  if(getpinfo(pstat) < 0) {
    printf("Error in ps.c\n");
    return -1;
  }

  printf("Name\tPID\tPPID\tPriority  State\tLength\n");

  for (int i = 0; i < NPROC; i++) {
    if (pstat->state[i] != UNUSED || pstat->pid[i] != 0) { 
      printf("%s\t%d\t%d\t%d\t  %d\t%d\n", pstat->name[i], pstat->pid[i], pstat->ppid[i], pstat->priority[i], pstat->state[i], pstat->length[i]);
    }
  }

  return 0;
}