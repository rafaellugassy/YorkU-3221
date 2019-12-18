#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int forkdepth = 10;
int val;
extern int sfork()
{
  if (--forkdepth >0)
    return fork();
  fprintf(stderr,"Too many forks\n");
  exit(-1);
}

// wait, but when the wait is over, it will add one to forkdepth
// after you close a process it should allow another one to be opened
// sfork was causing a lot of errors because of this not being implemented..
extern int swait(int *i)
{
	val = wait(i);
	forkdepth++;
	return val;
}
