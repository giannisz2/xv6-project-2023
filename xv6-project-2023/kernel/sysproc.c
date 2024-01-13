#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// Sets priority of current process to num
uint64 sys_setpriority(void) {
  int num; 
  argint(0, &num);
  if(num < 1 || num > 20) { // Failure
    printf("Error in sys_getpriority.\n");
    return -1;
  }
  struct proc* p = myproc();
  acquire(&p->lock);
  p->priority = num;
  release(&p->lock);

  return 0; // Success
}

// Gets info of process
uint64 sys_getpinfo(void) {
  uint64 pointer;
  struct proc* p = myproc();
  argaddr(0, &pointer);
  struct pstat* pstat = kalloc();
  //pstat = (struct pstat*)pointer;
  int x = copyout(p->pagetable, pointer, (char*)&pstat, sizeof(pstat)) ;
  if(x == -1) {
    printf("Error in sys_getpinfo.\n");
    return -1;
  }
  // Locks exist so another process wont run while this on is being copied
  int i = 0;
  for(p = proc; p < &proc[NPROC]; p++) {
  acquire(&p->lock);
    if(p->state == RUNNING) { // If process is active
      strncpy(pstat->name[i], p->name, sizeof(p->name));
      pstat->pid[i] = p->pid;
      pstat->ppid[i] = p->parent->pid;
      pstat->priority[i] = p->priority;
      pstat->state[i] = p->state;
      pstat->length[i] = p->sz;
    i++;
    }
  release(&p->lock);
  }
  return 0;
}

uint64 sys_printpinfo(void) {
  struct pstat* pstat = kalloc();
  argaddr(0, (uint64)pstat);
  pstat = (struct pstat*)pstat;
  printf("\tName\tPID\tPPID\tPriority\tState\tLength\n");
  for(int i = 0; i < NPROC; i++) {
    printf("%s\t%d\t%d\t%d\t%d\t%d\n", pstat->name[i], pstat->pid[i], pstat->ppid[i], pstat->priority[i], pstat->state[i], pstat->length[i]);
  }
}