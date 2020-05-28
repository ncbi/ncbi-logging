.PHONY: clean all run

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

all: log2src_ log2jsn_

#do not rm XXX_parser.*/XXX_scanner.*! this would deleted *.l and *.y too!
clean:
	rm -rf $(LIBDIR) $(BINDIR) $(OBJDIR)
	rm -f $(PREFIX_OP)parser.hpp $(PREFIX_OP)parser.cpp
	rm -f $(PREFIX_OP)scanner.hpp $(PREFIX_OP)scanner.cpp
	rm -f $(PREFIX_OP)parser.output
	rm -f $(PREFIX_AWS)parser.hpp $(PREFIX_AWS)parser.cpp
	rm -f $(PREFIX_AWS)scanner.hpp $(PREFIX_AWS)scanner.cpp
	rm -f $(PREFIX_AWS)parser.output
	rm -f $(PREFIX_GCP)parser.hpp $(PREFIX_GCP)parser.cpp
	rm -f $(PREFIX_GCP)scanner.hpp $(PREFIX_GCP)scanner.cpp
	rm -f $(PREFIX_GCP)parser.output

mkdir:
	@ mkdir -p $(LIBDIR) $(BINDIR) $(OBJDIR)

$(OBJDIR)/%.o: %.cpp
	$(CPP) $(CXXFLAGS) $< -o $@


$(OBJDIR)/$(PREFIX_OP)parser.o:  $(OBJDIR)/$(PREFIX_OP)scanner.o

$(OBJDIR)/$(PREFIX_AWS)parser.o:  $(OBJDIR)/$(PREFIX_AWS)scanner.o

$(OBJDIR)/$(PREFIX_GCP)parser.o:  $(OBJDIR)/$(PREFIX_GCP)scanner.o


$(PREFIX_OP)scanner.cpp: $(PREFIX_OP)scanner.l
	flex --header-file=$(PREFIX_OP)scanner.hpp --outfile=$(PREFIX_OP)scanner.cpp $(PREFIX_OP)scanner.l

$(PREFIX_OP)parser.cpp: $(PREFIX_OP)parser.y
	bison --report all -d $(PREFIX_OP)parser.y --output-file $(PREFIX_OP)parser.cpp


$(PREFIX_AWS)scanner.cpp: $(PREFIX_AWS)scanner.l
	flex --header-file=$(PREFIX_AWS)scanner.hpp --outfile=$(PREFIX_AWS)scanner.cpp $(PREFIX_AWS)scanner.l

$(PREFIX_AWS)parser.cpp: $(PREFIX_AWS)parser.y
	bison --report all -d $(PREFIX_AWS)parser.y --output-file $(PREFIX_AWS)parser.cpp


$(PREFIX_GCP)scanner.cpp: $(PREFIX_GCP)scanner.l
	flex --header-file=$(PREFIX_GCP)scanner.hpp --outfile=$(PREFIX_GCP)scanner.cpp $(PREFIX_GCP)scanner.l

$(PREFIX_GCP)parser.cpp: $(PREFIX_GCP)parser.y
	bison --report all -d $(PREFIX_GCP)parser.y --output-file $(PREFIX_GCP)parser.cpp

-include $(OBJDIR)/*.d

#################################################
# C++ log parsing library

LOG_LINES_SRC = \
	helper.cpp \
	$(PREFIX_OP)parser.cpp \
	$(PREFIX_OP)scanner.cpp \
	$(PREFIX_AWS)parser.cpp \
	$(PREFIX_AWS)scanner.cpp \
	$(PREFIX_GCP)parser.cpp \
	$(PREFIX_GCP)scanner.cpp

log_lines.o: $(LOG_LINES_SRC)

LOGLIB_OBJ = \
	$(PREFIX_OP)parser.o \
	$(PREFIX_OP)scanner.o \
	$(PREFIX_AWS)parser.o \
	$(PREFIX_AWS)scanner.o \
	$(PREFIX_GCP)parser.o \
	$(PREFIX_GCP)scanner.o \
	helper.o \
	log_lines.o

$(LIBDIR)/liblogparse.a: $(addprefix  $(OBJDIR)/,$(LOGLIB_OBJ))
	ar r $@ $+

lib liblogparse: mkdir $(LIBDIR)/liblogparse.a

#################################################
# minimal parser: stdin(any format)->parsing->stdout(csv)

LOG2SRCOBJ = log2src.o

$(BINDIR)/log2src: $(addprefix $(OBJDIR)/,$(LOG2SRCOBJ)) $(LIBDIR)/liblogparse.a
	$(LD) -o $@ $^

log2src_: mkdir $(BINDIR)/log2src

run2src: log2src_
	cat data/two_events.txt | $(BINDIR)/log2src >./data/run2src.out

test2src: log2src_
	mkdir -p actual
	cat data/two_events.txt | $(BINDIR)/log2src >./actual/run2src.out && diff ./expected/run2src.out ./actual/run2src.out
	rm -rf actual

#################################################
# tests
# assume that Googletests is installed and accessible

#build all test-binaries
tests: \
	test_helper \
	$(PREFIX_OP)test_flex \
	$(PREFIX_OP)test_parse \
	$(PREFIX_AWS)test_flex \
	$(PREFIX_AWS)test_parse \
	$(PREFIX_GCP)test_flex \
	$(PREFIX_GCP)test_parse

runparsetests:
	$(BINDIR)/$(PREFIX_OP)test_flex
	$(BINDIR)/$(PREFIX_OP)test_parse
	$(BINDIR)/$(PREFIX_AWS)test_flex
	$(BINDIR)/$(PREFIX_AWS)test_parse
	$(BINDIR)/$(PREFIX_GCP)test_flex
	$(BINDIR)/$(PREFIX_GCP)test_parse

runtests: liblogparse tests run_test_helper runparsetests test2src test2jsn

#################################################
# scanner - tests

run_test_helper: test_helper
	$(BINDIR)/test_helper

TEST_HELPER_OBJ = \
	test_helper.o 

$(BINDIR)/test_helper: $(addprefix  $(OBJDIR)/,$(TEST_HELPER_OBJ)) $(LIBDIR)/liblogparse.a
	$(LD) -o $@ $+ -lgtest -lpthread

test_helper: $(BINDIR)/test_helper

.PHONY: test_helper

#################################################
# scanner - tests

$(PREFIX_OP)flextest: lib $(PREFIX_OP)test_flex
	$(BINDIR)/$(PREFIX_OP)test_flex

$(PREFIX_AWS)flextest: lib $(PREFIX_AWS)test_flex
	$(BINDIR)/$(PREFIX_AWS)test_flex

$(PREFIX_GCP)flextest: lib $(PREFIX_GCP)test_flex
	$(BINDIR)/$(PREFIX_GCP)test_flex


OP_TEST_FLEX_OBJ = \
	$(PREFIX_OP)test_flex.o \
	$(PREFIX_OP)scanner.o

AWS_TEST_FLEX_OBJ = \
	$(PREFIX_AWS)test_flex.o \
	$(PREFIX_AWS)scanner.o

GCP_TEST_FLEX_OBJ = \
	$(PREFIX_GCP)test_flex.o \
	$(PREFIX_GCP)scanner.o

$(BINDIR)/$(PREFIX_OP)test_flex: $(addprefix  $(OBJDIR)/,$(OP_TEST_FLEX_OBJ))
	$(LD) -o $@ $+ -lgtest -lpthread

$(PREFIX_OP)test_flex: $(BINDIR)/$(PREFIX_OP)test_flex

$(BINDIR)/$(PREFIX_AWS)test_flex: $(addprefix  $(OBJDIR)/,$(AWS_TEST_FLEX_OBJ))
	$(LD) -o $@ $+ -lgtest -lpthread

$(PREFIX_AWS)test_flex: $(BINDIR)/$(PREFIX_AWS)test_flex

$(BINDIR)/$(PREFIX_GCP)test_flex: $(addprefix  $(OBJDIR)/,$(GCP_TEST_FLEX_OBJ))
	$(LD) -o $@ $+ -lgtest -lpthread

$(PREFIX_GCP)test_flex: $(BINDIR)/$(PREFIX_GCP)test_flex

.PHONY: $(PREFIX_OP)test_flex $(PREFIX_AWS)test_flex $(PREFIX_GCP)test_flex

#################################################
# parser - tests

$(PREFIX_OP)parsetest: lib $(PREFIX_OP)test_parse
	$(BINDIR)/$(PREFIX_OP)test_parse

$(PREFIX_AWS)parsetest: lib $(PREFIX_AWS)test_parse
	$(BINDIR)/$(PREFIX_AWS)test_parse

$(PREFIX_GCP)parsetest: lib $(PREFIX_GCP)test_parse
	$(BINDIR)/$(PREFIX_GCP)test_parse


OP_TEST_PARSE_OBJ = $(PREFIX_OP)test_parse.o log_lines.o

AWS_TEST_PARSE_OBJ = $(PREFIX_AWS)test_parse.o log_lines.o

GCP_TEST_PARSE_OBJ = $(PREFIX_GCP)test_parse.o log_lines.o


$(BINDIR)/$(PREFIX_OP)test_parse: $(addprefix $(OBJDIR)/,$(OP_TEST_PARSE_OBJ))
	$(LD) -o $@ $+ -lgtest -lpthread $(LIBDIR)/liblogparse.a

$(PREFIX_OP)test_parse: $(BINDIR)/$(PREFIX_OP)test_parse

$(BINDIR)/$(PREFIX_AWS)test_parse: $(addprefix $(OBJDIR)/,$(AWS_TEST_PARSE_OBJ))
	$(LD) -o $@ $+ -lgtest -lpthread $(LIBDIR)/liblogparse.a

$(PREFIX_AWS)test_parse: $(BINDIR)/$(PREFIX_AWS)test_parse

$(BINDIR)/$(PREFIX_GCP)test_parse: $(addprefix $(OBJDIR)/,$(GCP_TEST_PARSE_OBJ))
	$(LD) -o $@ $+ -lgtest -lpthread $(LIBDIR)/liblogparse.a

$(PREFIX_GCP)test_parse: $(BINDIR)/$(PREFIX_GCP)test_parse

.PHONY: $(PREFIX_OP)test_parse $(PREFIX_AWS)test_parse $(PREFIX_GCP)test_parse

#################################################
# minimal parser/Json: stdin(any format)->parsing->stdout(Json)

LOG2JSNOBJ = log2jsn.o

# Specify location of ncbi-vdb3/jwt-tool in JWT_TOOL if different
JWT_TOOL ?= ../../ncbi-vdb3/jwt-tool

CXXFLAGS += -I$(JWT_TOOL)/inc

$(BINDIR)/log2jsn: $(addprefix $(OBJDIR)/,$(LOG2JSNOBJ)) $(LIBDIR)/liblogparse.a
	$(LD) -o $@ $^ -L$(JWT_TOOL)/lib -lncbi-json-dbg -lncbi-secure-dbg -lutf8proc -lmbedcrypto

log2jsn_: mkdir $(BINDIR)/log2jsn

json: log2jsn_
	cat data/two_events.txt | $(BINDIR)/log2jsn 

test2jsn: log2jsn_
	# TODO
