.PHONY: clean run

BINDIR = bin
LIBDIR = lib
OBJDIR = obj

CC := gcc
CPP := g++
LD := g++
CXXFLAGS += -c -I . -g -MMD
PREFIX := log1_

all: log2csv_

clean:
	rm -rf *.o $(LIBDIR) $(BINDIR) $(OBJDIR) $(PREFIX)parser.* $(PREFIX)scanner.*

mkdir:
	@ mkdir -p $(LIBDIR) $(BINDIR) $(OBJDIR)

$(OBJDIR)/%.o: %.cpp
	$(CPP) $(CXXFLAGS) $< -o $@

$(OBJDIR)/$(PREFIX)parser.o:  $(OBJDIR)/$(PREFIX)scanner.o

$(PREFIX)scanner.cpp: parser.l
	flex --header-file=$(PREFIX)scanner.hpp --outfile=$(PREFIX)scanner.cpp parser.l

$(PREFIX)parser.cpp: parser.y
	bison --report all -d parser.y --output-file $(PREFIX)parser.cpp

-include $(OBJDIR)/*.d

#################################################
# C++ log parsing library

log_lines.o: $(PREFIX)parser.cpp $(PREFIX)scanner.cpp

LOGLIB_OBJ = \
	$(PREFIX)parser.o \
	$(PREFIX)scanner.o \
	log_lines.o \

$(LIBDIR)/liblogparse.a: $(addprefix  $(OBJDIR)/,$(LOGLIB_OBJ))
	ar r $@ $+

lib liblogparse: mkdir $(LIBDIR)/liblogparse.a

#################################################
# minimal parser: stdin(any format)->parsing->stdout(csv)

LOG2CSVOBJ = log2csv.o

$(BINDIR)/log2csv: $(addprefix $(OBJDIR)/,$(LOG2CSVOBJ)) $(LIBDIR)/liblogparse.a
	$(LD) -o $@ $^

log2csv_: mkdir $(BINDIR)/log2csv

#################################################
# tests
# assume that Googletests is installed and accessible

runtests: flextest parsetest

#################################################
# scanner

flextest: lib test_flex
	$(BINDIR)/test_flex

TEST_FLEX_OBJ = test_flex.o log1_scanner.o

test_flex: $(addprefix  $(OBJDIR)/,$(TEST_FLEX_OBJ))
	$(LD) -o $(BINDIR)/$@ $+ -lgtest -lpthread

#################################################
# parser

parsetest: lib test_parse
	$(BINDIR)/test_parse

TEST_PARSE_OBJ = test_parse.o log_lines.o

test_parse: $(addprefix $(OBJDIR)/,$(TEST_PARSE_OBJ))
	$(LD) -o $(BINDIR)/$@ $+ -lgtest -lpthread $(LIBDIR)/liblogparse.a

#run: $(PROG)
#	cat two_events.txt | ./$(PROG) > parsed.txt 2>&1
