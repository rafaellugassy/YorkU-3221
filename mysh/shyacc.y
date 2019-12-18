%{
#include <malloc.h>
#include <stdio.h>
#include <unistd.h>

#include "eval.h"
#include "proc.h"

#define YYSTYPE evptr
%}
%token CD EXIT FNAME NEWLINE AND OR
%%
command: list NEWLINE		{ shyyval = $1; YYACCEPT; }
	| list '<' FNAME NEWLINE { $$ = mk_evtree(REDINTAG,$1,
						 mk_evtree(SIMPLETAG,$3));
                                shyyval = $$;
				YYACCEPT; }
	| list '>' FNAME NEWLINE { $$ = mk_evtree(REDOUTAG,$1,
						 mk_evtree(SIMPLETAG,$3));
                                shyyval = $$;
				YYACCEPT; }
	| list '<' FNAME '>' FNAME NEWLINE
			{$$=mk_evtree(REDOUTAG,
				      mk_evtree(REDINTAG,$1,
					     mk_evtree(SIMPLETAG,$3)),
				      mk_evtree(SIMPLETAG,$5));
                                shyyval = $$;
				YYACCEPT; }
	| list '>' FNAME '<' FNAME NEWLINE
			{$$=mk_evtree(REDINTAG,
				      mk_evtree(REDOUTAG,$1,
					     mk_evtree(SIMPLETAG,$3)),
				      mk_evtree(SIMPLETAG,$5));
                                shyyval = $$;
			        YYACCEPT; }
	| error			{ YYABORT; }
;
list:	expr ';' list		{ 
                                  $$ = mk_evtree(SYNCHTAG,$1,$3);
				}
	| expr '&' list		{ 
                                  $$ = mk_evtree(ASYNCHTAG,$1,$3);
				}
	| expr			{ $$ = $1; }
;
expr: pipe AND expr		{ 
				  $$ = mk_evtree(ANDTAG,$1,$3);
				}
	| pipe OR expr		{ 
				  $$ = mk_evtree(ORTAG,$1,$3);
				}
	| pipe			{ $$ = $1; }
;
pipe: simple '|' pipe		{ 
				  $$ = mk_evtree(PIPETAG,$1,$3);
				}
	| simple		{ $$ = $1; }
;
simple: CD FNAME		{ 
				  $$ = mk_evtree(CDTAG,
						 mk_stringlist("cd",$2));
				}
	| EXIT			{ 
				  $$ = mk_evtree(EXITAG);
				}
	| fnamelist		{ $$ = $1; }
	|			{ $$ = mk_evtree(NULLTAG); }
	| '(' list ')'		{ $$ = $2; }
;
fnamelist: FNAME fnamelist	{ 
				  ((struct stringlist *)$1)->next = $2->leaf.data;
				  $2->leaf.data = (struct stringlist *)$1;
				  $$ = $2;
				}
	| FNAME			{ $$ = mk_evtree(SIMPLETAG,$1,$1); }
;
%%
#include "lex.yy.c"

int yyerror(str)
     char *str;
{
  printf("%s unexpected\n",str);
}

int yywrap()
{
  return 1;
}
