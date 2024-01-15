#include "param.h"

// Definition of pstat struct
struct pstat {
  int pid[NPROC]; // Process id
  int ppid[NPROC]; // Parent process id
  char name[NPROC][16]; // Name of process assuming 16 is the max length of characters for a process
  int priority[NPROC]; // Priority of process
  int state[NPROC]; // Current status of process
  int length[NPROC]; // Length of the process
};