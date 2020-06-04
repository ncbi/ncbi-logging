.PHONY: clean all run

BINDIR = bin
LIBDIR = lib
OBJDIR = obj

CC := gcc -c
CPP := g++ -c
LD := g++
CXXFLAGS += -I . -g -MMD

PREFIX_OP := op_
PREFIX_AWS := aws_
PREFIX_GCP := gcp_

all: log2jsn_

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

-include $(OBJDIR)/*.d

#################################################
# C++ log parsing library

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
# tests
# assume that Googletests is installed and accessible

runtests: liblogparse run_ut test2jsn

#build all test-binaries
tests: \
	$(BINDIR)/test_helper \
	$(BINDIR)/test_loglines \
	$(BINDIR)/$(PREFIX_OP)test_flex \
	$(BINDIR)/$(PREFIX_OP)test_parse \
	$(BINDIR)/$(PREFIX_AWS)test_flex \
	$(BINDIR)/$(PREFIX_AWS)test_parse \
	$(BINDIR)/$(PREFIX_GCP)test_flex \
	$(BINDIR)/$(PREFIX_GCP)test_parse

run_ut: tests
	$(BINDIR)/test_helper
	$(BINDIR)/test_loglines
	$(BINDIR)/$(PREFIX_OP)test_flex
	$(BINDIR)/$(PREFIX_OP)test_parse
	$(BINDIR)/$(PREFIX_AWS)test_flex
	$(BINDIR)/$(PREFIX_AWS)test_parse
	$(BINDIR)/$(PREFIX_GCP)test_flex
	$(BINDIR)/$(PREFIX_GCP)test_parse

#################################################
# helper - tests

TEST_HELPER_OBJ = \
	test_helper.o 

$(BINDIR)/test_helper: $(addprefix  $(OBJDIR)/,$(TEST_HELPER_OBJ)) $(LIBDIR)/liblogparse.a
	$(LD) -o $@ $+ -lgtest -lpthread

test_helper: $(BINDIR)/test_helper

run_test_helper: test_helper
	$(BINDIR)/test_helper

.PHONY: test_helper run_test_helper

#################################################
# loglines - tests

TEST_LOGLINES_OBJ = \
	test_loglines.o 

$(BINDIR)/test_loglines: $(addprefix  $(OBJDIR)/,$(TEST_LOGLINES_OBJ)) $(LIBDIR)/liblogparse.a
	$(LD) -o $@ $+ -lgtest -lpthread

test_loglines: $(BINDIR)/test_loglines

run_test_loglines: test_loglines
	$(BINDIR)/test_loglines

.PHONY: test_loglines run_test_loglines

#################################################
# scanner - tests

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

run_$(PREFIX_OP)flextest: lib $(PREFIX_OP)test_flex
	$(BINDIR)/$(PREFIX_OP)test_flex

run_$(PREFIX_AWS)flextest: lib $(PREFIX_AWS)test_flex
	$(BINDIR)/$(PREFIX_AWS)test_flex

run_$(PREFIX_GCP)flextest: lib $(PREFIX_GCP)test_flex
	$(BINDIR)/$(PREFIX_GCP)test_flex

.PHONY: run_$(PREFIX_OP)test_flex run_$(PREFIX_AWS)test_flex run_$(PREFIX_GCP)test_flex
.PHONY: $(PREFIX_OP)test_flex $(PREFIX_AWS)test_flex $(PREFIX_GCP)test_flex

#################################################
# parser - tests

OP_TEST_PARSE_OBJ = $(PREFIX_OP)test_parse.o log_lines.o
AWS_TEST_PARSE_OBJ = $(PREFIX_AWS)test_parse.o log_lines.o
GCP_TEST_PARSE_OBJ = $(PREFIX_GCP)test_parse.o log_lines.o


$(BINDIR)/$(PREFIX_OP)test_parse: $(addprefix $(OBJDIR)/,$(OP_TEST_PARSE_OBJ))
	$(LD) -o $@ $+ -lgtest -lpthread $(LIBDIR)/liblogparse.a

$(BINDIR)/$(PREFIX_AWS)test_parse: $(addprefix $(OBJDIR)/,$(AWS_TEST_PARSE_OBJ))
	$(LD) -o $@ $+ -lgtest -lpthread $(LIBDIR)/liblogparse.a

$(BINDIR)/$(PREFIX_GCP)test_parse: $(addprefix $(OBJDIR)/,$(GCP_TEST_PARSE_OBJ))
	$(LD) -o $@ $+ -lgtest -lpthread $(LIBDIR)/liblogparse.a


run_$(PREFIX_OP)parsetest: lib $(BINDIR)/$(PREFIX_OP)test_parse
	$(BINDIR)/$(PREFIX_OP)test_parse

run_$(PREFIX_AWS)parsetest: lib $(BINDIR)/$(PREFIX_AWS)test_parse
	$(BINDIR)/$(PREFIX_AWS)test_parse

run_$(PREFIX_GCP)parsetest: lib $(BINDIR)/$(PREFIX_GCP)test_parse
	$(BINDIR)/$(PREFIX_GCP)test_parse

.PHONY: run_$(PREFIX_OP)test_parse run_$(PREFIX_AWS)test_parse run_$(PREFIX_GCP)test_parse

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
	@mkdir -p actual
	cat data/op_two_events.txt    | $(BINDIR)/log2jsn op readable >./actual/op_two_events.out 2>./actual/op_two_events.err \
						&& diff ./expected/op_two_events.out ./actual/op_two_events.out \
						&& diff ./expected/op_two_events.err ./actual/op_two_events.err 
	cat data/aws_unrecognized.log | $(BINDIR)/log2jsn aws >./actual/aws_unrecognized.out 2>./actual/aws_unrecognized.err \
						&& diff ./expected/aws_unrecognized.out ./actual/aws_unrecognized.out \
						&& diff ./expected/aws_unrecognized.err ./actual/aws_unrecognized.err
	cat data/gcp_unrecognized.log | $(BINDIR)/log2jsn gcp >./actual/gcp_unrecognized.out 2>./actual/gcp_unrecognized.err \
						&& diff ./expected/gcp_unrecognized.out ./actual/gcp_unrecognized.out \
						&& diff ./expected/gcp_unrecognized.err ./actual/gcp_unrecognized.err
	@ echo "test2json passed"
	@rm -rf actual/run2jsn*
