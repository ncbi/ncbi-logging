.PHONY: clean run

CC := gcc
CPP := g++
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
	bison --report all -d parser.y --output-file $(PREFIX)parser.c

#################################################
# tests

# assume that Googletests is installed and accessible

runtest: test_flex
	./test_flex

CXXFLAGS += -I /usr/local/include

TEST_OBJ = test_flex.o log1_scanner.o

test_flex: $(TEST_OBJ)
	$(CPP) $(CFLAGS) -I /usr/local/include -o $@ $+ -lgtest -lpthread

