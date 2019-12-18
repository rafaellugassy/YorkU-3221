#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "eval.h"
#include "proc.h"
#include <fcntl.h>
extern int errno;

/**********************************************************************
		mk_evtree
***********************************************************************/

evptr mk_evtree(tag,left,right)
     int tag;
     void *left, *right;
{
  evptr res;
				/* returns a newly allocated node of    */
				/* an evtree. If the tag corresponds    */
				/* to a binary node both arguments are  */
				/* used. Accordingly for unary and leaf */
				/* nodes one or zero extra arguments    */
				/* are needed                           */
  res = (evptr) malloc(sizeof(union evtree));
  if (!res)
    {
      printf("mk_evtree: Not enough memory\n");
      exit(-1);
    };
  res->tag = tag;
  switch (tag)
    {
    case ORTAG:
    case ANDTAG:
    case LISTAG:
    case ASYNCHTAG:
    case SYNCHTAG:
    case PIPETAG:
    case REDINTAG:
    case REDOUTAG:
      res->son.left=left;
      res->son.right=right;
      return res;
    case SIMPLETAG:
    case CDTAG:
      res->leaf.data=left;
      return res;
    case NULLTAG:
    case EXITAG:
      return res;
    };
  return res;
}

/**********************************************************************
		mk_stringlist
***********************************************************************/

struct stringlist *mk_stringlist(str,nxt)
     char *str;
     struct stringlist *nxt;
{
  struct stringlist *res;
  char *tmpstr;
				/* return a newly allocated node         */
				/* of a string list that contains a copy */
				/* of str and nxt                        */
  res = (struct stringlist*) malloc(sizeof(struct stringlist));
  if (!res)
    {
      printf("mk_stringlist: Not enough memory\n");
      exit(-1);
    };

  tmpstr = (char*) malloc(strlen(str)+1);
  if (!tmpstr)
    {
      printf("mk_stringlist: Not enough memory\n");
      exit(-1);
    };
  strcpy(tmpstr, str);
  res->fname = tmpstr;
  res->next = nxt;
  return res;
};

/***********************************************************************
		printtree
************************************************************************/

void printtree(tree,depth)
     evptr tree;
     int depth;
{
  char *s;
  struct stringlist *scan;
  int i;
				/* Print the evtree sideways for debugging */
				/* purposes.                               */
				/* Most routines that accept an evtree     */
				/* as argument have this form              */
  switch (tree->tag)
    {
    case ORTAG:
      s = "||";
      goto printit;
    case ANDTAG:
      s = "&&";
      goto printit;
    case LISTAG:
      s = ";";
      goto printit;
    case ASYNCHTAG:
      s = "&";
      goto printit;
    case SYNCHTAG:
      s = ";";
      goto printit;
    case PIPETAG:
      s = "|";
      goto printit;
    case REDINTAG:
      s = "<";
      goto printit;
    case REDOUTAG:
      s = ">";
      goto printit;
    printit:
      printtree(tree->son.right,depth+1);
      for (i=0; i<depth; i++) printf("   ");
      printf("%s\n",s);
      printtree(tree->son.left,depth+1);
      break;
    case SIMPLETAG:
    case CDTAG:
      for (i=0; i<depth; i++) printf("   ");
      for (scan = tree->leaf.data; scan != NULL; scan = scan->next)
	printf("%s ",scan->fname);
      printf("\n");
      break;
    case NULLTAG:
      for (i=0; i<depth; i++) printf("   ");
      printf("<NULL>\n");
      break;
    case EXITAG:
      printf("exit\n");
      break;
    default:
      printf("Unknown tag %d\n",tree->tag);
      break;
    };
};


/***********************************************************************
		myExecute
************************************************************************/

// the return value of 0 is a fail, and 1 is a success in my method
int myExecute(tree, inport, outport)
	evptr tree;
	int inport, outport;
{
	struct stringlist *scan;
	char **argv;
	int size;
	int i = 0;
	int pid;
	int fd[2];
	switch (tree->tag)
    {
		// if the current tree node has && at the top
		case ANDTAG:
			// if the left tree is executed successfully, execute the right tree
			if (myExecute(tree->son.left, inport, outport))
				return myExecute(tree->son.right, inport, outport);
			// else return that the current method failed			
			else
				return 0;
		break;
		
		// if the current tree node has || at the top
		case ORTAG:
			// if the left tree is executed successfully, you're done
			if (myExecute(tree->son.left, inport, outport))
				return 1;
			// else execute the right tree	
			else
				return myExecute(tree->son.right, inport, outport);
		break;
		
		// if the current tree node has < at the top
		case REDINTAG:
			// open a file with the name that's to the right tree node 
			// | is done above <, so no need to worry about piping
			fd[0] = open(tree->son.right->leaf.data->fname, O_RDWR);
			
			// if the open fails, then print the error and return that the current method failed
			if (fd[0] < 0){
				fprintf(stderr, "%s: %s\n",tree->son.right->leaf.data->fname, strerror(errno));
				return 0;
			}
			
			// now execute the left tree with the file as inport
			return myExecute(tree->son.left, fd[0], outport);
		break;
		
		// if the current tree node has > at the top
		case REDOUTAG:
		
			// allocate memory for arv
			argv = (char **)malloc(sizeof(char *) * 3);
			// set argv[0] to be the touch command, so it will make a new file if it doesn't exist
			argv[0] = "touch";
			// set the name of the file to be the name of what's to the right
			argv[1] = tree->son.right->leaf.data->fname;
			argv[2] = NULL;

			// fork a new process
			pid = fork();
			
			// if the fork failed make print the error
			if(pid < 0){
				fprintf(stderr, "%s: fork failed: %s\n",argv[0], strerror(errno));
				return 0;
			}
			
			// if this is in the child process execute the touch method
			if(pid == 0){
				execvp(argv[0], argv);
				// if the execute fails, exit the child process
				exit(1);
			}
			// wait for child see forklib for swait
			swait(NULL);
			
			// now set fd[0] to be the file descriptor of the file we are trying to open
			fd[0] = open(tree->son.right->leaf.data->fname, O_RDWR);
			// execute the left tree with that file as the outport
			return myExecute(tree->son.left, inport, fd[0]);
		break;
		
		// if the current tree node has | at the top
		case PIPETAG:
		
			// do a pipe on fd, if it fails print the error, and return a fail
			if (pipe(fd) < 0){
				fprintf(stderr, "Error creating pipe: %s\n", strerror(errno));
				return 0;
    		}

			// fork a new process
			pid = fork();
			
			// if it fails print the error and return a fail
			if(pid < 0){
				fprintf(stderr, "fork failed: %s\n", strerror(errno));
				return 0;
			}
			
			// if the current process is the child
			if(pid == 0){
				// close the inport of fd for the child process
				close(fd[0]);
				
				// dup2 the fd[1] with the outport, and if it fails, print the error and exit
				if (dup2(fd[1], outport) < 0){
					fprintf(stderr, "dup2 failed: %s\n", strerror(errno));
					exit(0);
				}
				
				// execute, and exit with values depending if it failed or not
				if (myExecute(tree->son.left, inport, fd[1]) == 1)
					kill(getpid(), 1);
				else
					kill(getpid(), 0);
			}
			// wait for child process and save exit result into i see forklib for swait
			swait(&i);
			
			// close the outport for the child process, so the input can be read
			close(fd[1]);
			
			// if the return value of the child is a fail return a fail
			if (!i)
				return 0;
				
			// execute the right tree now
			return myExecute(tree->son.right, fd[0], outport);
			kill(getpid(), 1);
		break;
		
		// if the current tree node has & at the top
		case ASYNCHTAG:
		
			// create a new process
			pid = fork();
			
			// if it fails print the error, and return a fail
			if(pid < 0){
				fprintf(stderr, "fork failed: %s\n", strerror(errno));
				return 0;
			}
			
			// if the current process is the child, then execute the left tree
			if(pid == 0){
				myExecute(tree->son.left, inport, outport);
				
				// exit the process
				exit(1);
			}
			
			// if the current process is the parent execute the right tree
			else
				myExecute(tree->son.right, inport, outport);
			wait();
		break;
		
		// if the current tree node has ; at the top
		case SYNCHTAG:
			// execute the left tree, then execute the right tree in the same process
			myExecute(tree->son.left, inport, outport);
			myExecute(tree->son.right, inport, outport);
		break;
		
		// if the current tree node is a leaf
		case SIMPLETAG:
		
			// fork a new process
			pid = fork();
			
			// if it fails print the error and return a fail
			if(pid < 0){
				fprintf(stderr, "fork failed: %s\n", strerror(errno));
				exit(1);
			}
			
			// if the process is the child
			if(pid == 0){
				// if either the inport or the outport is not the stdin, and stdout 
				// dup the stdin and stdout to them
				if (inport != 0 || outport != 1){
					dup2(outport, 1);
					dup2(inport, 0);
				}
				
				// set the size to start at 1 (1 extra for the NULL)
				size = 1;
				
				// increase the size once for each next string in the list
				for (scan = tree->leaf.data; scan != NULL; scan = scan->next)
					size++;
					
				// allocate space for argv
				argv = (char **)(malloc(sizeof(char *) * size));
				
				// start i at 0
				i = 0;
				
				// set each value in argv to equal the corresponding string for the leaf
				for (scan = tree->leaf.data; scan != NULL; scan = scan->next)
					argv[i++] = scan->fname;
				// set the last value to NULL
				argv[i] = NULL;
				
				// execute argv
				execvp(argv[0], argv);
				
				// if it fails to execute argv, print the error
				fprintf(stderr, "%s: %s\n",argv[0], strerror(errno));
				// exit the child process as a fail
				kill(getpid(), 1);
			}
			else{
				// wait for the child and set i to the exit value see forklib
				swait(&i);
				// if the exit value is 0 then set i to 1, otherwise set i to zero, and return i
				if (i == 0)
					i = 1;
				else 
					i = 0;
				return i;
			}
			exit(1);
		break;
	}
	return 0;
}

