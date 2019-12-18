
#define fork()    sfork()

#define ORTAG     0
#define ANDTAG    1
#define LISTAG    2
#define ASYNCHTAG 3
#define SYNCHTAG  4
#define PIPETAG   5
#define REDINTAG  6
#define REDOUTAG  7
#define SIMPLETAG 8
#define NULLTAG   9
#define CDTAG     10
#define EXITAG    11

/*
    The above tags are used in the evtree (evaluation tree) tag field.
    The first 6 of them (0-5) correspond to compound commands like the
    OR command, the PIPE etc. We can access the two components of the command
    with tree->son.left and tree->son.right
    The other correspond to the simple command like 'cat eval.h' and the
    first word in there can be accessed with tree->leaf.data->fname 
*/

/*
    The following structure represents a linked list of strings. It is used
    to represent a simple command. For instance cd .. would consist of two
    linked records each one containing a pointer to a string. A new node
    is allocated by mk_stringlist.
*/
struct stringlist
{
  char *fname;
  struct stringlist *next;
};

/*
    The folowing structure can hold one node of the expression tree. It is
    a union, which means it can represent both a leaf and a nonleaf node.
    For an example of its use look at util.c. A new node is allocated by
    mk_evtree
*/
union evtree
{
  int tag;
  struct
	{
	  int tag;
	  union evtree *left, *right;
	} son;
  struct
	{
	  int tag;
	  struct stringlist *data;
	} leaf ;
};

typedef union evtree *evptr;

