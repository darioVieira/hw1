#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <signal.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

typedef struct process
{
  char** argv;//pass t (tok_t)
  int argc;//no idea what to store
  pid_t pid;//pass process ID
  int completed;//0 if not completed, 1 if completed
  int stopped;//0 if not stopped, 1 if stopped
  char background;//no idea what it means
  int status;//no idea
  struct termios tmodes;//no idea
  int stdin, stdout, stderr;//no idea
  struct process* nextProc;//pointer to next process}	doubly linked list
  struct process* prevProc;//pointer to previous process}
} process;

process* firstProcess; //pointer to the first process that is launched */

void launch_process(process* p);
void put_process_in_background (process* p, int cont);
void put_process_in_foreground (process* p, int cont);

#endif
