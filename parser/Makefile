.PHONY: clean run

BINDIR = bin
LIBDIR = lib
OBJDIR = obj

CC := gcc
CPP := g++
LD := g++
CXXFLAGS += -c -I . -g -MMD

PREFIX_OP := op_
PREFIX_AWS := aws_
PREFIX_GCP := gcp_

all: log2csv_

#do not rm XXX_parser.*/XXX_scanner.*! this would deleted *.l and *.y too!
clean:
	rm -rf $(LIBDIR) $(BINDIR) $(OBJDIR)
	rm -f $(PREFIX_OP)parser.hpp $(PREFIX_OP)parser.cpp
	rm -f $(PREFIX_OP)scanner.hpp $(PREFIX_OP)scanner.cpp
	rm -f $(PREFIX_OP)parser.output
	rm -f $(PREFIX_AWS)parser.hpp $(PREFIX_AWS)parser.cpp
	rm -f $(PREFIX_AWS)scanner.hpp $(PREFIX_AWS)scanner.cpp
	rm -f $(PREFIX_AWS)parser.output

mkdir:
	@ mkdir -p $(LIBDIR) $(BINDIR) $(OBJDIR)

$(OBJDIR)/%.o: %.cpp
	$(CPP) $(CXXFLAGS) $< -o $@

$(OBJDIR)/$(PREFIX_OP)parser.o:  $(OBJDIR)/$(PREFIX_OP)scanner.o

$(OBJDIR)/$(PREFIX_AWS)parser.o:  $(OBJDIR)/$(PREFIX_AWS)scanner.o

$(PREFIX_OP)scanner.cpp: $(PREFIX_OP)scanner.l
	flex --header-file=$(PREFIX_OP)scanner.hpp --outfile=$(PREFIX_OP)scanner.cpp $(PREFIX_OP)scanner.l

$(PREFIX_OP)parser.cpp: $(PREFIX_OP)parser.y
	bison --report all -d $(PREFIX_OP)parser.y --output-file $(PREFIX_OP)parser.cpp

$(PREFIX_AWS)scanner.cpp: $(PREFIX_AWS)scanner.l
	flex --header-file=$(PREFIX_AWS)scanner.hpp --outfile=$(PREFIX_AWS)scanner.cpp $(PREFIX_AWS)scanner.l

$(PREFIX_AWS)parser.cpp: $(PREFIX_AWS)parser.y
	bison --report all -d $(PREFIX_AWS)parser.y --output-file $(PREFIX_AWS)parser.cpp

-include $(OBJDIR)/*.d

#################################################
# C++ log parsing library

LOG_LINES_SRC = \
	$(PREFIX_OP)parser.cpp \
	$(PREFIX_OP)scanner.cpp \
	$(PREFIX_AWS)parser.cpp \
	$(PREFIX_AWS)scanner.cpp

log_lines.o: $(LOG_LINES_SRC)

LOGLIB_OBJ = \
	$(PREFIX_OP)parser.o \
	$(PREFIX_OP)scanner.o \
	$(PREFIX_AWS)parser.o \
	$(PREFIX_AWS)scanner.o \
	helper.o \
	log_lines.o

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

#build all test-binaries
tests: $(PREFIX_OP)test_flex \
	$(PREFIX_OP)test_parse \
	$(PREFIX_AWS)test_flex \
	$(PREFIX_AWS)test_parse 

runtests:
	$(BINDIR)/$(PREFIX_OP)test_flex
	$(BINDIR)/$(PREFIX_OP)test_parse
	$(BINDIR)/$(PREFIX_AWS)test_flex
	$(BINDIR)/$(PREFIX_AWS)test_parse

#################################################
# scanner - tests

$(PREFIX_OP)flextest: lib $(PREFIX_OP)test_flex
	$(BINDIR)/$(PREFIX_OP)test_flex

$(PREFIX_AWS)flextest: lib $(PREFIX_AWS)test_flex
	$(BINDIR)/$(PREFIX_AWS)test_flex

OP_TEST_FLEX_OBJ = \
	$(PREFIX_OP)test_flex.o \
	$(PREFIX_OP)scanner.o

AWS_TEST_FLEX_OBJ = \
	$(PREFIX_AWS)test_flex.o \
	$(PREFIX_AWS)scanner.o

$(PREFIX_OP)test_flex: $(addprefix  $(OBJDIR)/,$(OP_TEST_FLEX_OBJ))
	$(LD) -o $(BINDIR)/$@ $+ -lgtest -lpthread

$(PREFIX_AWS)test_flex: $(addprefix  $(OBJDIR)/,$(AWS_TEST_FLEX_OBJ))
	$(LD) -o $(BINDIR)/$@ $+ -lgtest -lpthread

#################################################
# parser - tests

$(PREFIX_OP)parsetest: lib $(PREFIX_OP)test_parse
	$(BINDIR)/$(PREFIX_OP)test_parse

$(PREFIX_AWS)parsetest: lib $(PREFIX_AWS)test_parse
	$(BINDIR)/$(PREFIX_AWS)test_parse

OP_TEST_PARSE_OBJ = $(PREFIX_OP)test_parse.o log_lines.o

AWS_TEST_PARSE_OBJ = $(PREFIX_AWS)test_parse.o log_lines.o

$(PREFIX_OP)test_parse: $(addprefix $(OBJDIR)/,$(OP_TEST_PARSE_OBJ))
	$(LD) -o $(BINDIR)/$@ $+ -lgtest -lpthread $(LIBDIR)/liblogparse.a

$(PREFIX_AWS)test_parse: $(addprefix $(OBJDIR)/,$(AWS_TEST_PARSE_OBJ))
	$(LD) -o $(BINDIR)/$@ $+ -lgtest -lpthread $(LIBDIR)/liblogparse.a
