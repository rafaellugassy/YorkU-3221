#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "eval.h"
#include "proc.h"

evptr shyyval;

int main(argc,argv)
     int argc;
     char **argv;
{
  int pid, temp;

  printf("Welcome to my little shell\n");
  printf(">> ");
  while (!yyparse())
    {
      pid = eval(shyyval,0,1);
      do temp = wait(NULL); while (temp>=0);
      printf(">> ");
    }
  printf("please learn how to type correctly and try again!\n");
}

