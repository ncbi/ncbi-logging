.PHONY: clean run

CC := gcc
CFLAGS := -g
PREFIX := log1_
PROG = $(PREFIX)prog

all: $(PROG)

OBJ = \
	$(PREFIX)parser.o \
	$(PREFIX)scanner.o \
	parselib.o \
	parser-functions.o \
	main.o

$(PROG) : $(OBJ)
	$(CC) $(CFLAGS) -o $@ $+

clean:
	rm -f $(PROG) $(OBJ) $(PREFIX)parser.? $(PREFIX)scanner.?

run: $(PROG)
	cat two_events.txt | ./$(PROG) > parsed.txt 2>&1

$(PREFIX)parser.o: $(PREFIX)scanner.o

$(PREFIX)scanner.c: parser.l
	flex --header-file=$(PREFIX)scanner.h --outfile=$(PREFIX)scanner.c parser.l

$(PREFIX)parser.c: parser.y
	bison -d parser.y --output-file $(PREFIX)parser.c
