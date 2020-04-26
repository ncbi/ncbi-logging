PROG = parser

all: $(PROG)

$(PROG).tab.c $(PROG).tab.h: $(PROG).y
	bison -t -v -d $(PROG).y

lex.yy.c: $(PROG).l $(PROG).tab.h
	flex $(PROG).l

$(PROG): lex.yy.c $(PROG).tab.c $(PROG).tab.h
	gcc -o $(PROG) $(PROG).tab.c lex.yy.c

clean:
	rm $(PROG) $(PROG).tab.c lex.yy.c $(PROG).tab.h $(PROG).output

run: $(PROG)
	cat two_events.txt | ./$(PROG)
