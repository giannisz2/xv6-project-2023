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
int setpriority(int num) {
  struct proc* p;

  if(p == 0) // Error checking
    return -1;
 
  if(num < 1 || num > 20) { // Check for valid numbers
    printf("Priority number invalid. Setting 10 as default.\n");
    p->priority = 10;
    return -1; 
  }
  p->priority = num;
  printf("The process %s has priority %d", p->name, p->priority);
  return 0;
}

// Gets info of process
int getpinfo(struct pstat* pstat) {
  printf("Copying data..\n");
  struct proc* p;
  if(p == 0 || pstat) // Error checking
    return -1;
  
  // Locks exist so another process wont run while this on is being copied
  acquire(&p->lock);
  for(int i = 0; i < NPROC; i++) {
    if(p == RUNNING) { // If process is active
      strncpy(pstat->name[i], p->name, sizeof(p->name));
      pstat->pid[i] = p->pid;
      pstat->ppid[i] = p->parent->pid;
      pstat->priority[i] = p->priority;
      pstat->state[i] = p->state;
      pstat->length[i] = p->sz;
    } else {
      pstat->pid[i] = 0; // IF process isn't active place the number 0 as a pid as an indication 
    }
  }
  release(&p->lock);
  return 0;
}