.PHONY: clean run

CC := gcc
CPP := g++
CFLAGS := -g
CXXFLAGS += -I /usr/local/include -I . -g
PREFIX := log1_

all: liblogparse.a

clean:
	rm -f *.o $(PREFIX)parser.* $(PREFIX)scanner.* liblogparse.a

$(PREFIX)parser.o: $(PREFIX)scanner.o

$(PREFIX)scanner.cpp: parser.l
	flex --header-file=$(PREFIX)scanner.hpp --outfile=$(PREFIX)scanner.cpp parser.l

$(PREFIX)parser.cpp: parser.y
	bison --report all -d parser.y --output-file $(PREFIX)parser.cpp

#################################################
# C++ log parsing library

log_lines.o: $(PREFIX)parser.cpp $(PREFIX)scanner.cpp

LOGLIB_OBJ = \
	$(PREFIX)parser.o \
	$(PREFIX)scanner.o \
	log_lines.o \

liblogparse.a: $(LOGLIB_OBJ)
	ar r $@ $+

lib: liblogparse.a

#################################################
# tests
# assume that Googletests is installed and accessible

runtests: flextest parsetest

#################################################
# scanner

flextest: test_flex
	./test_flex

TEST_FLEX_OBJ = test_flex.o log1_scanner.o

test_flex: $(TEST_FLEX_OBJ)
	$(CPP) $(CFLAGS) -I /usr/local/include -o $@ $+ -lgtest -lpthread

#################################################
# parser

parsetest: test_parse
	./test_parse

TEST_PARSE_OBJ = test_parse.o log_lines.o

test_parse: $(TEST_PARSE_OBJ) liblogparse.a
	$(CPP) $(CFLAGS) -I /usr/local/include -o $@ $+ -lgtest -lpthread

#run: $(PROG)
#	cat two_events.txt | ./$(PROG) > parsed.txt 2>&1
