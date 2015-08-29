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


#include "io.h"
#include "parse.h"
#include "process.h"
#include "shell.h"


process *currProc;


int main()
{
	printf("0\n");
	char *temp="hello here there";
	char **s=temp;
	tok_t *t="hello there";
	printf("1\n");
	
	printf("2\n");
	//currProc->argv=t;
	printf("3\n");
	currProc.pid=5;
	currProc.completed=0;
    currProc.stopped=0;
    printf("4\n");
	printf("Command:\t%s\nProcess ID:\t%i\nCompleted:\t%i\t(0 if not completed, 1 if completed)\nStopped:\t%i\t(0 if not stopped, 1 if stopped)",currProc->argv,currProc->pid,currProc->completed,currProc->stopped);

	/*char temp[10];
	int i;
	scanf("%s %i",temp,&i);
	printf("::%s::\n",temp);
   /*char *invalid_characters = ">";
   char *myString = "hello.c > temp.txt";
   char *c = invalid_characters;
   int reFrom=0, wrTo=0,min=0,i;
   //printf("here\n");
   for( i = 0; myString[i]; i++) 
   {
   	//printf("%i\t%c\n",i,myString[i]);
    reFrom=i;//if(strchr(myString[i],'<')) reFrom = i;
   	wrTo=8;//if(strchr(myString[i],'>')) wrTo = i;
   	min = (reFrom < wrTo) ? reFrom : wrTo;// min =min value of reFrom and wrTo
   	printf("reFrom:\t%i\nwrTo:\t%i\nwc min:\t%i\n",reFrom,wrTo,min);
   }

   
   
   
   
   /*if(strchr(myString,*c))
   	printf("here\n");
   
   while (*c)
   {
   	   printf("Here:%c\n",*c);
       if (strchr(invalid_characters, *c))
       {
          printf("%c is in \"%s\"\n", *c, myString);
       }

       c++;
   }*/

   return 0;
}
