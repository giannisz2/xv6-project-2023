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
  struct proc* proc;
 
  if(num < 1 || num > 20) { // Check for valid numbers
    printf("Priority number invalid. Setting 10 as default.\n");
    proc->priority = 10;
    return -1; 
  }
  proc->priority = num;
  printf("The process %s has priority %d", proc->name, proc->priority);
  return 0;
}

// Gets info of process
int getpinfo(struct pstat* pstat) {
  printf("Copying data..\n");
  struct proc* proc;
  pstat->name = proc->name;
  pstat->pid = proc->pid;
  pstat->ppid = proc->parent->pid;
  pstat->priority = proc->priority;
  pstat->state = proc->state;
  pstat->length = proc->sz;
  return 0;
}