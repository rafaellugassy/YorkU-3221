#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>

#include "eval.h"
#include "proc.h"

int eval(tree,inport,outport)
     evptr tree;
     int inport, outport;
{
	// execute the myExecute method located in util.c
	myExecute(tree, inport, outport);
	return 0;
};
