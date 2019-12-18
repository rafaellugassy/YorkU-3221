FILENAME	([-a-zA-Z./,~0-9+@#])
%{

%}
%%
cd		{ yylval = (evptr) mk_stringlist(yytext,NULL); return CD; }
exit|quit	{ yylval = (evptr) mk_stringlist(yytext,NULL); return EXIT; }
{FILENAME}*	{ yylval = (evptr) mk_stringlist(yytext,NULL); return FNAME; }
"&&"		return AND;
"||"		return OR;
"\n"		return NEWLINE;
[&|;<>()]	return *yytext;
[ \t\n]		;
%%
