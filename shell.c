//References: Craig, Meelan & Nathan all aided me.

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <fcntl.h>

#define INPUT_STRING_SIZE 80

#include "io.h"
#include "parse.h"
#include "process.h"
#include "shell.h"

char *cwd[PATH_MAX];
bool *procBegun;

int cmd_quit(tok_t arg[]) 
{
  printf("Bye\n");
  exit(0);
  return 1;
}

int cmd_cd(tok_t arg[])
{	
	char *dir=arg[0];
	
	if(chdir((dir[0]=='-')?cwd:dir)<0)//catches an invalid directory
		perror("Unable to change");
	getcwd(cwd,sizeof(cwd));
	return 1;
} 
  
int cmd_lsp(tok_t arg[])
{
	
	process *currProc=firstProcess;
	while(currProc)
	{
		fprintf(stdout,"Command:\t%s\nProcess ID:\t%i\nCompleted:\t%i\t(0 if not completed, 1 if completed)\nStopped:\t%i\t(0 if not stopped, 1 if stopped)\n",currProc->argv,currProc->pid,currProc->completed,currProc->stopped);
		currProc=currProc->nextProc;
	}
	return 1;
}


//Will return 0 if read/write file could not be opened
int execPath(char *fname, char **argv) //does not currently redirect stoud of UNIX commands, only normal functions //wc works... but ls doesn't... these are the questions that plague me
{
   int reFrom=0, wrTo=0,i;
   
   for( i = 0; argv[i]; i++) 
   {
    if(strchr(argv[i],'>')) //only works if entered in the specific layout:dsd [function] [space] > [space] [write to folder]
    	wrTo = i;//stores the pointer to the begining of the char* that has >, for the file that must be written to, we need i+1
   	if(strchr(argv[i],'<')) 
   		reFrom = i; 
   }
   
   if(wrTo)//means if int!=0
   {
	   //dup2 code and explanation obtained from codewiki.wikidot.com/c:system-calls:dup2
	   int file = open(argv[wrTo+1],O_WRONLY);//opens as a write only file;
	   if(file < 0)    
	   {
	   	fprintf(stderr,"Could not access: %s\n",argv[wrTo+1]);
	   	return 0;
	   }
	   //Now we redirect standard output to the file using dup2 
	   if(dup2(file,1) < 0)//1 parameter in dup2 means write to
	   {
	   	fprintf(stderr,"Could not redirect standard output to file\n");
	   	return 0;
	   }
	}
	if(reFrom)
	{
		int file=open(argv[reFrom+1],O_RDONLY);
		if(file<0)
		{
			fprintf(stderr,"Could not access: %s\n",argv[reFrom+1]);
			return 0;
		}
		if(dup2(file,0)<0)//0 parameter in dup2 means read from
		{
			fprintf(stderr,"Could not read standard input from file\n");
			return 0;
		}
	}
   
   
   
   char temp[PATH_MAX];//PATH_MAX is a macro that sets the size of temp to the longest possible size the OS allows a path to be- apparently not true though...
   tok_t *path;
   char *envLs = getenv("PATH");//searches the environment list (can be seen with echo `getenv("PATH")`) and returns a pointer to the corresponding value string   
   path = getToks(envLs);//cannot all be done in 1 line for some reason... (tok_t*path)
   
   int execvRetVal=(execv(fname,argv));//runs execv in current path first, if executable is not in current path, execv returns -1
   
   if( execvRetVal==-1) 
   { 
     for(int i = 0; i < MAXTOKS && path[i]; i++) //adds all the pieces from the above pointer for the path resolution
     {
       strcpy(temp, path[i]);
       strcat(temp, "/");
       strcat(temp, fname);
       execv(temp,argv);
     }
   }
   return 1;
 }
 

 

int cmd_help(tok_t arg[]);


/* Command Lookup table */
typedef int cmd_fun_t (tok_t args[]); /* cmd functions take token array and return int */
typedef struct fun_desc 
{
  cmd_fun_t *fun;
  char *cmd;
  char *doc;
} fun_desc_t;

fun_desc_t cmd_table[] = 
{
  {cmd_help, "?", "\tShow this help menu"},
  {cmd_quit, "quit", "\tQuit the command shell"},
  {cmd_cd, "cd", "\tChange the directory"},
  {cmd_lsp, "lsp","\tList all processes that have been run in the current shell session"},
};

int cmd_help(tok_t arg[]) 
{
  int i;
  for (i=0; i < (sizeof(cmd_table)/sizeof(fun_desc_t)); i++) 
  {
    printf("%s - %s\n",cmd_table[i].cmd, cmd_table[i].doc);
  }
  return 1;
}

int lookup(char cmd[]) 
{
  int i;
  for (i=0; i < (sizeof(cmd_table)/sizeof(fun_desc_t)); i++) 
  {
    if (cmd && (strcmp(cmd_table[i].cmd, cmd) == 0)) return i;
  }
  return -1;
}

void init_shell()
{
  /* Check if we are running interactively */
  shell_terminal = STDIN_FILENO;

  /** Note that we cannot take control of the terminal if the shell
      is not interactive */
  shell_is_interactive = isatty(shell_terminal);

  if(shell_is_interactive)
  {

    /* force into foreground */
    while(tcgetpgrp (shell_terminal) != (shell_pgid = getpgrp()))
      kill( - shell_pgid, SIGTTIN);

    shell_pgid = getpid();
    /* Put shell in its own process group */
    if(setpgid(shell_pgid, shell_pgid) < 0)
    {
      fprintf(stdout,"Couldn't put the shell in its own process group");
      exit(1);
    }

    /* Take control of the terminal */
    tcsetpgrp(shell_terminal, shell_pgid);
    tcgetattr(shell_terminal, &shell_tmodes);
  }
  /** YOUR CODE HERE */
  
}

/**
 * Add a process to our process list*/
void addProcess(struct process* p, int procCount)
{
  /** YOUR CODE HERE */
  
  if(!procBegun)
  {
  	firstProcess=p;
  	procBegun=true;
  }
  else
  {
  	process *currProc,*prevProc;
  	currProc=firstProcess;
  	while(currProc)
  	{
  		prevProc=currProc;
  		currProc=currProc->nextProc;
  	}
  	if(prevProc)//making sure prevProc exists- i.e. for when theres only 1 option
  		prevProc->nextProc=p;
  	currProc=p;
  	currProc->prevProc=prevProc;
  }
}

/**
 * Creates a process given the inputString from stdin
 */
process* createProcess(char **inputString,int pid)
{
		
  process *p=malloc(sizeof(struct process));
  
  
  /*char *command="";
  int i=0;
  while(inputString[i])
  {
  	fprintf(stdout,"%i\n",i);
  	command=strcat(command,inputString[i]);
  	fprintf(stdout,"::\n");
  	i++;
  }*/
  
  p->argv=inputStrin[0];
  p->pid=pid;
  p->completed=0;
  p->stopped=0;
  return p;
}

int shell (int argc, char *argv[]) 
{
  char *s = malloc(INPUT_STRING_SIZE+1);			/* user input string */
  tok_t *t;			/* tokens parsed from input */
  int lineNum = 0;
  int fundex = -1;
  pid_t pid = getpid();		/* get current processes PID */
  pid_t ppid = getppid();	/* get parents PID */
  pid_t cpid, tcpid, cpgid;
  
  init_shell();
  
  int *procCount=malloc(20);
  
  firstProcess=malloc(sizeof(struct process));
  procCount=0;
  procBegun=false;
  
  //size_t size;
  getcwd(cwd,sizeof(cwd));//enters the absolute path name into the array pointed to by wd, size is the size of the array wd points to
  
  printf("%s running as PID %d under %d\n",argv[0],pid,ppid);
  
  lineNum=0;
  fprintf(stdout, "%d: %s$ ", lineNum, cwd);
  while ((s = freadln(stdin)))
  {
  	lineNum++;
    t = getToks(s); /* break the line into tokens */
    fundex = lookup(t[0]); /* Is first token a shell literal */
    
    if(fundex >= 0) cmd_table[fundex].fun(&t[1]);
    else 
    {
	  addProcess(createProcess(t,pid), procCount);
	  cpid=fork();
	  
	  if(cpid==0)//is the child
	  {
	  //add new process
	  pid = getpid();
	  
	  //completed adding new process
	  	int execRes=execPath(t[0],t);
	  	if(!execRes)
	  		fprintf(stdout,"Please ensure file name/s was/were entered correctly\n");
	  	exit(0);
      }
      wait(cpid);//makes parent wait for child~ I think~ WORKS
      procCount++;
      
    }
    
    fprintf(stdout, "%d: %s$ ", lineNum,cwd);
  }
  return 0;
}
