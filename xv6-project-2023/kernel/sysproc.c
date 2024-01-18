#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "pstat.h"

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
  if(num < 1 || num > 20) { 
    printf("Priority out of bounds.\n");
    return -1; // Failure
  }
  struct proc* process = myproc(); // Current proc struct
  acquire(&process->lock);
  process->priority = num;
  release(&process->lock);

  return 0; // Success
}

// Gets info of process
int sys_getpinfo(void) {
  struct proc* my_proc = myproc();
  struct proc* process;
  struct pstat *u_pstat;   
  
  argaddr(0, (uint64*)&u_pstat);
  
  struct pstat k_pstat = {0}; // Initialize k_pstat

  acquire(&wait_lock); // We need global lock for process table access so others CPU's won't be able to cause race conditions
  int i = 0;
  for(process = proc; process < &proc[NPROC]; process++) {
    if(process->state != UNUSED) { 
      k_pstat.pid[i] = process->pid;
      if(process->parent)
        k_pstat.ppid[i] = process->parent->pid;
      else 
        k_pstat.ppid[i] = -1;
      strncpy(k_pstat.name[i], process->name, sizeof(process->name));
      k_pstat.priority[i] = process->priority;
      k_pstat.state[i] = process->state;
      k_pstat.length[i] = process->sz;
    }
    i++;
  }
  release(&wait_lock); 

  // Copy kernel struct to user struct
  if(copyout(my_proc->pagetable, (uint64)u_pstat, (char *)&k_pstat, sizeof(struct pstat)) == -1) {
    printf("Couldn't copy kernel struct to user struct.\n");
    return -1;
  }

  return 0;
}