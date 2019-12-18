HDR = eval.h proc.h
SRC = y.tab.c forklib.c util.c eval.c main.c
OBJ = y.tab.o forklib.o util.o eval.o main.o
LEX = flex -l
YACC = bison -y
LEXFILE = shlex.l
YACCFILE = shyacc.y
CFLAGS= -c -g

mysh: $(OBJ)
	gcc -o mysh $(OBJ)

y.tab.o:  y.tab.c

$(SRC): $(HDR)

y.tab.c: lex.yy.c $(YACCFILE)
	yacc -v $(YACCFILE)

lex.yy.c: $(LEXFILE)
	$(LEX) $(LEXFILE)

clean:
	rm -f y.tab.c lex.yy.c mysh y.tab.o forklib.o util.o eval.o main.o

tar:
	tar cf mysh.tar forklib.c util.c eval.c main.c shlex.l shyacc.y mysh-manual.ascii eval.h proc.h makefile

man:
	less mysh-manual.ascii
