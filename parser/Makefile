.PHONY: clean all run prof

BINDIR = bin
LIBDIR = lib
OBJDIR = obj

CC := gcc -c
CPP := g++ -c
LD := g++
CXXFLAGS += -I . -MMD
GPROFFLAGS = -pg -O3
FLEXFLAGS = -8 --fast

PREFIX_OP := op_
PREFIX_OP_V2 := op_v2_
PREFIX_AWS := aws_
PREFIX_AWS_V2 := aws_v2_
PREFIX_GCP := gcp_
PREFIX_GCP_V2 := gcp_v2_
PREFIX_TW := tw_
PREFIX_TW_V2 := tw_v2_

all: log2jsn_
	@echo "done"

prof: log2jsn_prof

#do not rm XXX_parser.*/XXX_scanner.*! this would deleted *.l and *.y too!
clean:
	rm -rf $(LIBDIR) $(BINDIR) $(OBJDIR)
	rm -f $(PREFIX_OP)parser.hpp $(PREFIX_OP)parser.cpp
	rm -f $(PREFIX_OP)scanner.hpp $(PREFIX_OP)scanner.cpp
	rm -f $(PREFIX_OP)parser.output
	rm -f $(PREFIX_OP_V2)parser.hpp $(PREFIX_OP_V2)parser.cpp
	rm -f $(PREFIX_OP_V2)scanner.hpp $(PREFIX_OP_V2)scanner.cpp
	rm -f $(PREFIX_OP_V2)parser.output
	rm -f $(PREFIX_AWS)parser.hpp $(PREFIX_AWS)parser.cpp
	rm -f $(PREFIX_AWS)scanner.hpp $(PREFIX_AWS)scanner.cpp
	rm -f $(PREFIX_AWS)parser.output
	rm -f $(PREFIX_AWS_V2)parser.hpp $(PREFIX_AWS_V2)parser.cpp
	rm -f $(PREFIX_AWS_V2)scanner.hpp $(PREFIX_AWS_V2)scanner.cpp
	rm -f $(PREFIX_AWS_V2)parser.output
	rm -f $(PREFIX_GCP)parser.hpp $(PREFIX_GCP)parser.cpp
	rm -f $(PREFIX_GCP)scanner.hpp $(PREFIX_GCP)scanner.cpp
	rm -f $(PREFIX_GCP)parser.output
	rm -f $(PREFIX_GCP_V2)parser.hpp $(PREFIX_GCP_V2)parser.cpp
	rm -f $(PREFIX_GCP_V2)scanner.hpp $(PREFIX_GCP_V2)scanner.cpp
	rm -f $(PREFIX_GCP_V2)parser.output
	rm -f $(PREFIX_TW_V2)parser.hpp $(PREFIX_TW_V2)parser.cpp
	rm -f $(PREFIX_TW_V2)scanner.hpp $(PREFIX_TW_V2)scanner.cpp
	rm -f $(PREFIX_TW_V2)parser.output

mkdir:
	@ mkdir -p $(LIBDIR) $(BINDIR) $(OBJDIR)

$(OBJDIR)/%.dbg.o: %.cpp
	$(CPP) $(CXXFLAGS) -g $< -o $(OBJDIR)/$(*F).dbg.o

$(OBJDIR)/%.rel.o: %.cpp
	$(CPP) $(CXXFLAGS) -O3 $< -o $(OBJDIR)/$(*F).rel.o

$(OBJDIR)/%.prof.o: %.cpp
	$(CPP) $(CXXFLAGS) $(GPROFFLAGS) $< -o $(OBJDIR)/$(*F).prof.o

-include $(OBJDIR)/*.d

#################################################
# C++ log parsing library

$(OBJDIR)/$(PREFIX_OP)parser.dbg.o:  $(OBJDIR)/$(PREFIX_OP)scanner.dbg.o
$(OBJDIR)/$(PREFIX_OP)parser.rel.o:  $(OBJDIR)/$(PREFIX_OP)scanner.rel.o
$(OBJDIR)/$(PREFIX_OP)parser.prof.o:  $(OBJDIR)/$(PREFIX_OP)scanner.prof.o

$(OBJDIR)/$(PREFIX_AWS)parser.dbg.o:  $(OBJDIR)/$(PREFIX_AWS)scanner.dbg.o
$(OBJDIR)/$(PREFIX_AWS)parser.rel.o:  $(OBJDIR)/$(PREFIX_AWS)scanner.rel.o
$(OBJDIR)/$(PREFIX_AWS)parser.prof.o:  $(OBJDIR)/$(PREFIX_AWS)scanner.prof.o

$(OBJDIR)/$(PREFIX_GCP)parser.dbg.o:  $(OBJDIR)/$(PREFIX_GCP)scanner.dbg.o
$(OBJDIR)/$(PREFIX_GCP)parser.rel.o:  $(OBJDIR)/$(PREFIX_GCP)scanner.rel.o
$(OBJDIR)/$(PREFIX_GCP)parser.prof.o:  $(OBJDIR)/$(PREFIX_GCP)scanner.prof.o

$(PREFIX_OP)scanner.cpp: $(PREFIX_OP)scanner.l $(PREFIX_OP)parser.hpp
	flex $(FLEXFLAGS) --header-file=$(PREFIX_OP)scanner.hpp --outfile=$(PREFIX_OP)scanner.cpp $(PREFIX_OP)scanner.l

$(PREFIX_OP)parser.cpp: $(PREFIX_OP)parser.y
	bison --report all -d $(PREFIX_OP)parser.y --output-file $(PREFIX_OP)parser.cpp

$(PREFIX_AWS)scanner.cpp: $(PREFIX_AWS)scanner.l $(PREFIX_AWS)parser.hpp
	flex $(FLEXFLAGS) --header-file=$(PREFIX_AWS)scanner.hpp --outfile=$(PREFIX_AWS)scanner.cpp $(PREFIX_AWS)scanner.l

$(PREFIX_AWS)parser.cpp: $(PREFIX_AWS)parser.y
	bison --report all -d $(PREFIX_AWS)parser.y --output-file $(PREFIX_AWS)parser.cpp

$(PREFIX_GCP)scanner.cpp: $(PREFIX_GCP)scanner.l $(PREFIX_GCP)parser.hpp
	flex $(FLEXFLAGS) --header-file=$(PREFIX_GCP)scanner.hpp --outfile=$(PREFIX_GCP)scanner.cpp $(PREFIX_GCP)scanner.l

$(PREFIX_GCP)parser.cpp: $(PREFIX_GCP)parser.y
	bison --report all -d $(PREFIX_GCP)parser.y --output-file $(PREFIX_GCP)parser.cpp

LOG_LINES_SRC = \
	helper.cpp \
	$(PREFIX_OP)parser.cpp \
	$(PREFIX_OP)scanner.cpp \
	$(PREFIX_AWS)parser.cpp \
	$(PREFIX_AWS)scanner.cpp \
	$(PREFIX_GCP)parser.cpp \
	$(PREFIX_GCP)scanner.cpp \
 	log_lines.cpp \

LOGLIB_DBG_OBJ = $(subst .cpp,.dbg.o,$(LOG_LINES_SRC))
LOGLIB_REL_OBJ = $(subst .cpp,.rel.o,$(LOG_LINES_SRC))
LOGLIB_PROF_OBJ = $(subst .cpp,.prof.o,$(LOG_LINES_SRC))

$(LIBDIR)/liblogparse-dbg.a: $(addprefix  $(OBJDIR)/,$(LOGLIB_DBG_OBJ))
	ar r $@ $+

$(LIBDIR)/liblogparse-rel.a: $(addprefix  $(OBJDIR)/,$(LOGLIB_REL_OBJ))
	ar r $@ $+

$(LIBDIR)/liblogparse-prof.a: $(addprefix  $(OBJDIR)/,$(LOGLIB_PROF_OBJ))
	ar r $@ $+

lib: mkdir $(LIBDIR)/liblogparse-dbg.a $(LIBDIR)/liblogparse-rel.a

.PHONY: lib

#################################################
# tests
# assume that Googletests is installed and accessible

runtests: run_ut test2jsn

#build all test-binaries
tests: \
	$(BINDIR)/test_helper-dbg \
	$(BINDIR)/test_loglines-dbg \
	$(BINDIR)/$(PREFIX_OP)test_flex-dbg \
	$(BINDIR)/$(PREFIX_OP)test_parse-dbg \
	$(BINDIR)/$(PREFIX_AWS)test_flex-dbg \
	$(BINDIR)/$(PREFIX_AWS)test_parse-dbg \
	$(BINDIR)/$(PREFIX_GCP)test_flex-dbg \
	$(BINDIR)/$(PREFIX_GCP)test_parse-dbg

run_ut: tests
	$(BINDIR)/test_helper-dbg
	$(BINDIR)/test_loglines-dbg
	$(BINDIR)/$(PREFIX_OP)test_flex-dbg
	$(BINDIR)/$(PREFIX_OP)test_parse-dbg
	$(BINDIR)/$(PREFIX_AWS)test_flex-dbg
	$(BINDIR)/$(PREFIX_AWS)test_parse-dbg
	$(BINDIR)/$(PREFIX_GCP)test_flex-dbg
	$(BINDIR)/$(PREFIX_GCP)test_parse-dbg

#################################################
# helper - tests

TEST_HELPER_SRC = \
	test_helper.cpp

TEST_HELPER_DBG_OBJ = $(addprefix  $(OBJDIR)/,$(subst .cpp,.dbg.o,$(TEST_HELPER_SRC)))
TEST_HELPER_REL_OBJ = $(addprefix  $(OBJDIR)/,$(subst .cpp,.rel.o,$(TEST_HELPER_SRC)))

$(BINDIR)/test_helper-dbg: $(TEST_HELPER_DBG_OBJ) $(LIBDIR)/liblogparse-dbg.a
	$(LD) -o $@ $+ -lgtest -lpthread

$(BINDIR)/test_helper-rel: $(TEST_HELPER_REL_OBJ) $(LIBDIR)/liblogparse-rel.a
	$(LD) -o $@ $+ -lgtest -lpthread

test_helper: mkdir $(BINDIR)/test_helper-dbg $(BINDIR)/test_helper-rel

run_test_helper: test_helper
	$(BINDIR)/test_helper-dbg
	$(BINDIR)/test_helper-rel

.PHONY: test_helper run_test_helper

#################################################
# loglines - tests

TEST_LOGLINES_SRC = \
	test_loglines.cpp

TEST_LOGLINES_DBG_OBJ = $(addprefix  $(OBJDIR)/,$(subst .cpp,.dbg.o,$(TEST_LOGLINES_SRC)))
TEST_LOGLINES_REL_OBJ = $(addprefix  $(OBJDIR)/,$(subst .cpp,.rel.o,$(TEST_LOGLINES_SRC)))

$(BINDIR)/test_loglines-dbg: $(TEST_LOGLINES_DBG_OBJ) $(LIBDIR)/liblogparse-dbg.a
	$(LD) -o $@ $+ -lgtest -lpthread

$(BINDIR)/test_loglines-rel: $(TEST_LOGLINES_REL_OBJ) $(LIBDIR)/liblogparse-rel.a
	$(LD) -o $@ $+ -lgtest -lpthread

test_loglines: mkdir $(BINDIR)/test_loglines-dbg $(BINDIR)/test_loglines-rel

run_test_loglines: test_loglines
	$(BINDIR)/test_loglines-dbg
	$(BINDIR)/test_loglines-rel

.PHONY: test_loglines run_test_loglines

#################################################
# scanner - tests

# On Premises
OP_TEST_FLEX_SRC = \
	$(PREFIX_OP)test_flex.cpp \
	$(PREFIX_OP)scanner.cpp

OP_TEST_FLEX_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$(OP_TEST_FLEX_SRC)))
OP_TEST_FLEX_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$(OP_TEST_FLEX_SRC)))

$(BINDIR)/$(PREFIX_OP)test_flex-dbg: $(OP_TEST_FLEX_DBG_OBJ)
	$(LD) -o $@ $+ -lgtest -lpthread

$(BINDIR)/$(PREFIX_OP)test_flex-rel: $(OP_TEST_FLEX_REL_OBJ)
	$(LD) -o $@ $+ -lgtest -lpthread

run_$(PREFIX_OP)flextest: lib $(BINDIR)/$(PREFIX_OP)test_flex-dbg $(BINDIR)/$(PREFIX_OP)test_flex-rel
	$(BINDIR)/$(PREFIX_OP)test_flex-dbg
	$(BINDIR)/$(PREFIX_OP)test_flex-rel

.PHONY: run_$(PREFIX_OP)flextest

# AWS

AWS_TEST_FLEX_SRC = \
	$(PREFIX_AWS)test_flex.cpp \
	$(PREFIX_AWS)scanner.cpp

AWS_TEST_FLEX_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$(AWS_TEST_FLEX_SRC)))
AWS_TEST_FLEX_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$(AWS_TEST_FLEX_SRC)))

$(BINDIR)/$(PREFIX_AWS)test_flex-dbg: $(AWS_TEST_FLEX_DBG_OBJ)
	$(LD) -o $@ $+ -lgtest -lpthread

$(BINDIR)/$(PREFIX_AWS)test_flex-rel: $(AWS_TEST_FLEX_REL_OBJ)
	$(LD) -o $@ $+ -lgtest -lpthread

run_$(PREFIX_AWS)flextest: lib $(BINDIR)/$(PREFIX_AWS)test_flex-dbg $(BINDIR)/$(PREFIX_AWS)test_flex-rel
	$(BINDIR)/$(PREFIX_AWS)test_flex-dbg
	$(BINDIR)/$(PREFIX_AWS)test_flex-rel

.PHONY: run_$(PREFIX_AWS)flextest

#GCP

GCP_TEST_FLEX_SRC = \
	$(PREFIX_GCP)test_flex.cpp \
	$(PREFIX_GCP)scanner.cpp

GCP_TEST_FLEX_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$(GCP_TEST_FLEX_SRC)))
GCP_TEST_FLEX_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$(GCP_TEST_FLEX_SRC)))

$(BINDIR)/$(PREFIX_GCP)test_flex-dbg: $(GCP_TEST_FLEX_DBG_OBJ)
	$(LD) -o $@ $+ -lgtest -lpthread

$(BINDIR)/$(PREFIX_GCP)test_flex-rel: $(GCP_TEST_FLEX_SRC)
	$(LD) -o $@ $+ -lgtest -lpthread

run_$(PREFIX_GCP)flextest: lib $(BINDIR)/$(PREFIX_GCP)test_flex-dbg $(BINDIR)/$(PREFIX_GCP)test_flex-rel
	$(BINDIR)/$(PREFIX_GCP)test_flex-dbg
	$(BINDIR)/$(PREFIX_GCP)test_flex-rel

.PHONY: run_$(PREFIX_GCP)flextest

#################################################
# parser - tests

# On Premises

OP_TEST_PARSE_SRC = \
	$(PREFIX_OP)test_parse.cpp

OP_TEST_PARSE_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$(OP_TEST_PARSE_SRC)))
OP_TEST_PARSE_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$(OP_TEST_PARSE_SRC)))

$(BINDIR)/$(PREFIX_OP)test_parse-dbg: $(OP_TEST_PARSE_DBG_OBJ) lib
	$(LD) -o $@ $< -lgtest -lpthread $(LIBDIR)/liblogparse-dbg.a

$(BINDIR)/$(PREFIX_OP)test_parse-rel: $(OP_TEST_PARSE_REL_OBJ) lib
	$(LD) -o $@ $< -lgtest -lpthread $(LIBDIR)/liblogparse-rel.a

run_$(PREFIX_OP)parsetest: mkdir $(BINDIR)/$(PREFIX_OP)test_parse-dbg $(BINDIR)/$(PREFIX_OP)test_parse-rel
	$(BINDIR)/$(PREFIX_OP)test_parse-dbg
	$(BINDIR)/$(PREFIX_OP)test_parse-rel

.PHONY: run_$(PREFIX_OP)test_parse

# AWS

AWS_TEST_PARSE_SRC = \
	$(PREFIX_AWS)test_parse.cpp

AWS_TEST_PARSE_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$(AWS_TEST_PARSE_SRC)))
AWS_TEST_PARSE_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$(AWS_TEST_PARSE_SRC)))

$(BINDIR)/$(PREFIX_AWS)test_parse-dbg: $(AWS_TEST_PARSE_DBG_OBJ) lib
	$(LD) -o $@ $< -lgtest -lpthread $(LIBDIR)/liblogparse-dbg.a

$(BINDIR)/$(PREFIX_AWS)test_parse-rel: $(AWS_TEST_PARSE_REL_OBJ) lib
	$(LD) -o $@ $< -lgtest -lpthread $(LIBDIR)/liblogparse-rel.a

run_$(PREFIX_AWS)parsetest: $(BINDIR)/$(PREFIX_AWS)test_parse-dbg $(BINDIR)/$(PREFIX_AWS)test_parse-rel
	$(BINDIR)/$(PREFIX_AWS)test_parse-dbg
	$(BINDIR)/$(PREFIX_AWS)test_parse-rel

.PHONY: run_$(PREFIX_AWS)test_parse

# GCP

GCP_TEST_PARSE_SRC = \
	$(PREFIX_GCP)test_parse.cpp

GCP_TEST_PARSE_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$(GCP_TEST_PARSE_SRC)))
GCP_TEST_PARSE_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$(GCP_TEST_PARSE_SRC)))

$(BINDIR)/$(PREFIX_GCP)test_parse-dbg: $(GCP_TEST_PARSE_DBG_OBJ) lib
	$(LD) -o $@ $< -lgtest -lpthread $(LIBDIR)/liblogparse-dbg.a

$(BINDIR)/$(PREFIX_GCP)test_parse-rel: $(GCP_TEST_PARSE_REL_OBJ) lib
	$(LD) -o $@ $< -lgtest -lpthread $(LIBDIR)/liblogparse-rel.a

run_$(PREFIX_GCP)parsetest: $(BINDIR)/$(PREFIX_GCP)test_parse-dbg $(BINDIR)/$(PREFIX_GCP)test_parse-rel
	$(BINDIR)/$(PREFIX_GCP)test_parse-dbg
	$(BINDIR)/$(PREFIX_GCP)test_parse-rel

.PHONY: run_$(PREFIX_GCP)test_parse

#################################################
# Specify location of ncbi-vdb3/jwt-tool in JWT_TOOL if different
JWT_TOOL ?= ../../ncbi-vdb3/jwt-tool

CXXFLAGS += -I$(JWT_TOOL)/inc -I$(JWT_TOOL)/utf8proc -I$(JWT_TOOL)/tool

#################################################
# VDB3 Command line parsing
CMD_SRC_LOCATION = $(JWT_TOOL)/tool
CMD_SRC = $(CMD_SRC_LOCATION)/cmdline.cpp

DBG_CMD_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$(notdir $(CMD_SRC))))
REL_CMD_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$(notdir $(CMD_SRC))))

$(OBJDIR)/%.dbg.o: $(CMD_SRC_LOCATION)/%.cpp
	$(CPP) $(CXXFLAGS) -g $< -o $(OBJDIR)/$(*F).dbg.o

$(OBJDIR)/%.rel.o: $(CMD_SRC_LOCATION)/%.cpp
	$(CPP) $(CXXFLAGS) -O3 $< -o $(OBJDIR)/$(*F).rel.o

#################################################
# minimal parser/Json: stdin(any format)->parsing->stdout(Json)

LOG2JSN_SRC = log2jsn.cpp

LOG2JSN_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$(LOG2JSN_SRC)))
LOG2JSN_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$(LOG2JSN_SRC)))
LOG2JSN_PROF_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.prof.o,$(LOG2JSN_SRC)))

$(BINDIR)/log2jsn-dbg: $(LOG2JSN_DBG_OBJ) $(DBG_CMD_OBJ) $(LIBDIR)/liblogparse-dbg.a
	$(LD) -o $@ $^ -L$(JWT_TOOL)/lib -lncbi-json-dbg -lncbi-secure-dbg -lutf8proc -lmbedcrypto

$(BINDIR)/log2jsn-rel: $(LOG2JSN_REL_OBJ) $(REL_CMD_OBJ) $(LIBDIR)/liblogparse-rel.a
	$(LD) -o $@ $^ -L$(JWT_TOOL)/lib -lncbi-json-rel -lncbi-secure-rel-no-atomic -lutf8proc -lmbedcrypto

$(BINDIR)/log2jsn-prof: $(LOG2JSN_PROF_OBJ) $(DBG_CMD_OBJ) $(LIBDIR)/liblogparse-prof.a
	$(LD) -o $@ $^ $(GPROFFLAGS) -L$(JWT_TOOL)/lib -lncbi-json-rel -lncbi-secure-rel-no-busy -lutf8proc -lmbedcrypto

log2jsn_: mkdir $(BINDIR)/log2jsn-dbg $(BINDIR)/log2jsn-rel

log2jsn_prof: mkdir $(BINDIR)/log2jsn-prof

# acceptance

# which version to run for acceptance (uncomment one)
LOG2JSON = $(BINDIR)/log2jsn-dbg
#LOG2JSON = $(BINDIR)/log2jsn-rel

test2jsn: log2jsn_
	@ mkdir -p actual
	jq -e . expected/*.out > /dev/null
	cat data/$(PREFIX_OP)testlines.log    | $(LOG2JSON) op -p -t >./actual/$(PREFIX_OP)testlines.out 2>./actual/$(PREFIX_OP)testlines.err \
						&& diff ./expected/$(PREFIX_OP)testlines.out ./actual/$(PREFIX_OP)testlines.out \
						&& diff ./expected/$(PREFIX_OP)testlines.err ./actual/$(PREFIX_OP)testlines.err
	cat data/$(PREFIX_AWS)testlines.log | $(LOG2JSON) aws -t >./actual/$(PREFIX_AWS)testlines.out 2>./actual/$(PREFIX_AWS)testlines.err \
						&& diff ./expected/$(PREFIX_AWS)testlines.out ./actual/$(PREFIX_AWS)testlines.out \
						&& diff ./expected/$(PREFIX_AWS)testlines.err ./actual/$(PREFIX_AWS)testlines.err
	cat data/$(PREFIX_GCP)testlines.log | $(LOG2JSON) gcp -t >./actual/$(PREFIX_GCP)testlines.out 2>./actual/$(PREFIX_GCP)testlines.err \
						&& diff ./expected/$(PREFIX_GCP)testlines.out ./actual/$(PREFIX_GCP)testlines.out \
						&& diff ./expected/$(PREFIX_GCP)testlines.err ./actual/$(PREFIX_GCP)testlines.err
	cat data/$(PREFIX_OP)testlines.log  | $(LOG2JSON) op -p -j -t --readable >./actual/$(PREFIX_OP)testlines_pretty.out 2>./actual/$(PREFIX_OP)testlines.err \
						&& diff ./expected/$(PREFIX_OP)testlines_pretty.out ./actual/$(PREFIX_OP)testlines_pretty.out \
						&& diff ./expected/$(PREFIX_OP)testlines.err ./actual/$(PREFIX_OP)testlines.err
	cat data/$(PREFIX_OP)testlines.log | $(LOG2JSON) op -l2 -t > ./actual/op_filtered.out 2>./actual/op_filtered.err \
						&& diff ./expected/op_filtered.out ./actual/op_filtered.out \
						&& diff ./expected/op_filtered.err ./actual/op_filtered.err
	cat data/$(PREFIX_AWS)testlines.log | $(LOG2JSON) aws -l2 -t > ./actual/aws_filtered.out 2>./actual/aws_filtered.err \
						&& diff ./expected/aws_filtered.out ./actual/aws_filtered.out \
						&& diff ./expected/aws_filtered.err ./actual/aws_filtered.err
	cat data/$(PREFIX_GCP)testlines.log | $(LOG2JSON) gcp -l2 -t > ./actual/gcp_filtered.out 2>./actual/gcp_filtered.err \
						&& diff ./expected/gcp_filtered.out ./actual/gcp_filtered.out \
						&& diff ./expected/gcp_filtered.err ./actual/gcp_filtered.err

	@ echo "test2json passed"
	@ rm -rf actual/run2jsn*

.PHONY: log2jsn_ log2jsn_prof test2jsn

#################################################
# unit tests for formatter/catwriter

TEST_SUPPORT_SRC = \
	Formatters.cpp test_formatter.cpp \
	CatWriters.cpp test_catwriter.cpp \
	Queues.cpp test_queues.cpp \
	test_support_main.cpp

TEST_SUPPORT_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$(TEST_SUPPORT_SRC)))
TEST_SUPPORT_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$(TEST_SUPPORT_SRC)))

$(BINDIR)/test_support-dbg: $(TEST_SUPPORT_DBG_OBJ)
	$(LD) -o $@ $^ -L$(JWT_TOOL)/lib -lncbi-json-dbg -lncbi-secure-dbg -lutf8proc -lmbedcrypto -lgtest -lpthread

$(BINDIR)/test_support-rel: $(TEST_SUPPORT_REL_OBJ)
	$(LD) -o $@ $^ -L$(JWT_TOOL)/lib -lncbi-json-rel -lncbi-secure-rel -lutf8proc -lmbedcrypto -lgtest -lpthread

test_support: mkdir $(BINDIR)/test_support-dbg $(BINDIR)/test_support-rel

run_test_support: mkdir $(BINDIR)/test_support-dbg $(BINDIR)/test_support-rel
	$(BINDIR)/test_support-dbg
	$(BINDIR)/test_support-rel

.PHONY: test_support run_test_support

#################################################
# V2 parsers

COMMON_V2_SRC = \
	CatWriters.cpp \
	Formatters.cpp \
	ReceiverInterface.cpp \
	Queues.cpp

DBG_TOOL_OBJ = $(OBJDIR)/Tool.dbg.o $(DBG_CMD_OBJ)
REL_TOOL_OBJ = $(OBJDIR)/Tool.rel.o $(REL_CMD_OBJ)
PROF_TOOL_OBJ = $(OBJDIR)/Tool.prof.o $(PROF_CMD_OBJ)

#################################################
# stand alone AWS parser

AWS2JSN_CMN_SRC = \
	$(COMMON_V2_SRC) \
	$(PREFIX_AWS_V2)parser.cpp \
	$(PREFIX_AWS_V2)scanner.cpp \
	AWS_Interface.cpp

AWS2JSN_SRC = $(AWS2JSN_CMN_SRC) aws2jsn.cpp

AWS2JSN_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$(AWS2JSN_SRC)))
AWS2JSN_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$(AWS2JSN_SRC)))
AWS2JSN_PROF_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.prof.o,$(AWS2JSN_SRC)))

$(OBJDIR)/$(PREFIX_AWS_V2)parser.dbg.o:  	$(OBJDIR)/$(PREFIX_AWS_V2)scanner.dbg.o
$(OBJDIR)/$(PREFIX_AWS_V2)parser.rel.o:  	$(OBJDIR)/$(PREFIX_AWS_V2)scanner.rel.o
$(OBJDIR)/$(PREFIX_AWS_V2)parser.prof.o:  	$(OBJDIR)/$(PREFIX_AWS_V2)scanner.prof.o

$(PREFIX_AWS_V2)scanner.cpp: $(PREFIX_AWS_V2)scanner.l $(PREFIX_AWS_V2)parser.hpp
	flex $(FLEXFLAGS) --header-file=$(PREFIX_AWS_V2)scanner.hpp --outfile=$(PREFIX_AWS_V2)scanner.cpp $(PREFIX_AWS_V2)scanner.l

$(PREFIX_AWS_V2)parser.cpp: $(PREFIX_AWS_V2)parser.y
	bison --report all -d $(PREFIX_AWS_V2)parser.y --output-file $(PREFIX_AWS_V2)parser.cpp

$(BINDIR)/aws2jsn-dbg: $(AWS2JSN_DBG_OBJ) $(DBG_TOOL_OBJ)
	$(LD) -o $@ $^ -L$(JWT_TOOL)/lib -lncbi-json-dbg -lncbi-secure-dbg -lutf8proc -lmbedcrypto -lpthread

$(BINDIR)/aws2jsn-rel: $(AWS2JSN_REL_OBJ) $(REL_TOOL_OBJ)
	$(LD) -o $@ $^ -L$(JWT_TOOL)/lib -lncbi-json-rel -lncbi-secure-rel -lutf8proc -lmbedcrypto -lpthread

$(BINDIR)/aws2jsn-prof: $(AWS2JSN_PROF_OBJ) $(PROF_TOOL_OBJ)
	$(LD) -o $@ $^ $(GPROFFLAGS) -L$(JWT_TOOL)/lib -lncbi-json-rel -lncbi-secure-rel -lutf8proc -lmbedcrypto -lpthread

aws: mkdir $(BINDIR)/aws2jsn-dbg $(BINDIR)/aws2jsn-rel

aws_prof: mkdir $(BINDIR)/aws2jsn-prof

$(PREFIX_AWS_V2)_TEST_PARSE_SRC = $(AWS2JSN_CMN_SRC) $(PREFIX_AWS_V2)test_parse.cpp
$(PREFIX_AWS_V2)_TEST_FLEX_SRC = $(PREFIX_AWS_V2)test_flex.cpp $(PREFIX_AWS_V2)scanner.cpp

$(PREFIX_AWS_V2)_TEST_PARSE_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$($(PREFIX_AWS_V2)_TEST_PARSE_SRC)))
$(PREFIX_AWS_V2)_TEST_PARSE_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$($(PREFIX_AWS_V2)_TEST_PARSE_SRC)))

$(PREFIX_AWS_V2)_TEST_FLEX_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$($(PREFIX_AWS_V2)_TEST_FLEX_SRC)))
$(PREFIX_AWS_V2)_TEST_FLEX_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$($(PREFIX_AWS_V2)_TEST_FLEX_SRC)))

$(BINDIR)/$(PREFIX_AWS_V2)test_parse-dbg: $($(PREFIX_AWS_V2)_TEST_PARSE_DBG_OBJ)
	$(LD) -o $@ $^ -lgtest -lpthread -L$(JWT_TOOL)/lib -lncbi-json-dbg -lncbi-secure-dbg -lutf8proc -lmbedcrypto

$(BINDIR)/$(PREFIX_AWS_V2)test_parse-rel: $($(PREFIX_AWS_V2)_TEST_PARSE_REL_OBJ)
	$(LD) -o $@ $^ -lgtest -lpthread -L$(JWT_TOOL)/lib -lncbi-json-rel -lncbi-secure-rel -lutf8proc -lmbedcrypto

$(BINDIR)/$(PREFIX_AWS_V2)test_flex-dbg: $($(PREFIX_AWS_V2)_TEST_FLEX_DBG_OBJ)
	$(LD) -o $@ $^ -lgtest -lpthread

$(BINDIR)/$(PREFIX_AWS_V2)test_flex-rel: $($(PREFIX_AWS_V2)_TEST_FLEX_REL_OBJ)
	$(LD) -o $@ $^ -lgtest -lpthread

run_$(PREFIX_AWS_V2)parse_test: run_test_support run_$(PREFIX_AWS_V2)flex_test $(BINDIR)/$(PREFIX_AWS_V2)test_parse-dbg $(BINDIR)/$(PREFIX_AWS_V2)test_parse-rel
	$(BINDIR)/$(PREFIX_AWS_V2)test_parse-dbg
	$(BINDIR)/$(PREFIX_AWS_V2)test_parse-rel

run_$(PREFIX_AWS_V2)flex_test: $(BINDIR)/$(PREFIX_AWS_V2)test_flex-dbg $(BINDIR)/$(PREFIX_AWS_V2)test_flex-rel
	$(BINDIR)/$(PREFIX_AWS_V2)test_flex-dbg
	$(BINDIR)/$(PREFIX_AWS_V2)test_flex-rel

.PHONY: aws aws_prof run_$(PREFIX_AWS_V2)test_parse run_$(PREFIX_AWS_V2)test_flex

#################################################
# stand alone OP parser

OP2JSN_CMN_SRC = \
	$(COMMON_V2_SRC) \
	$(PREFIX_OP_V2)parser.cpp \
	$(PREFIX_OP_V2)scanner.cpp \
	OP_Interface.cpp

OP2JSN_SRC = $(OP2JSN_CMN_SRC) op2jsn.cpp

OP2JSN_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$(OP2JSN_SRC)))
OP2JSN_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$(OP2JSN_SRC)))
OP2JSN_PROF_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.prof.o,$(OP2JSN_SRC)))

$(OBJDIR)/$(PREFIX_OP_V2)parser.dbg.o:  $(OBJDIR)/$(PREFIX_OP_V2)scanner.dbg.o
$(OBJDIR)/$(PREFIX_OP_V2)parser.rel.o:  $(OBJDIR)/$(PREFIX_OP_V2)scanner.rel.o
$(OBJDIR)/$(PREFIX_OP_V2)parser.prof.o:  $(OBJDIR)/$(PREFIX_OP_V2)scanner.prof.o

$(PREFIX_OP_V2)scanner.cpp: $(PREFIX_OP_V2)scanner.l $(PREFIX_OP_V2)parser.hpp
	flex $(FLEXFLAGS) --header-file=$(PREFIX_OP_V2)scanner.hpp --outfile=$(PREFIX_OP_V2)scanner.cpp $(PREFIX_OP_V2)scanner.l

$(PREFIX_OP_V2)parser.cpp: $(PREFIX_OP_V2)parser.y
	bison --report all -d $(PREFIX_OP_V2)parser.y --output-file $(PREFIX_OP_V2)parser.cpp

$(BINDIR)/op2jsn-dbg: $(OP2JSN_DBG_OBJ) $(DBG_TOOL_OBJ)
	$(LD) -o $@ $^ -L$(JWT_TOOL)/lib -lncbi-json-dbg -lncbi-secure-dbg -lutf8proc -lmbedcrypto -lpthread

$(BINDIR)/op2jsn-rel: $(OP2JSN_REL_OBJ) $(REL_TOOL_OBJ)
	$(LD) -o $@ $^ -L$(JWT_TOOL)/lib -lncbi-json-rel -lncbi-secure-rel -lutf8proc -lmbedcrypto -lpthread

$(BINDIR)/op2jsn-prof: $(OP2JSN_PROF_OBJ) $(DBG_TOOL_OBJ)
	$(LD) -o $@ $^ $(GPROFFLAGS) -L$(JWT_TOOL)/lib -lncbi-json-rel -lncbi-secure-rel -lutf8proc -lmbedcrypto -lpthread

op: mkdir $(BINDIR)/op2jsn-dbg $(BINDIR)/op2jsn-rel

op_prof: mkdir $(BINDIR)/op2jsn-prof

$(PREFIX_OP_V2)_TEST_PARSE_SRC = $(OP2JSN_CMN_SRC) $(PREFIX_OP_V2)test_parse.cpp
$(PREFIX_OP_V2)_TEST_FLEX_SRC = $(PREFIX_OP_V2)test_flex.cpp $(PREFIX_OP_V2)scanner.cpp

$(PREFIX_OP_V2)_TEST_PARSE_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$($(PREFIX_OP_V2)_TEST_PARSE_SRC)))
$(PREFIX_OP_V2)_TEST_PARSE_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$($(PREFIX_OP_V2)_TEST_PARSE_SRC)))

$(PREFIX_OP_V2)_TEST_FLEX_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$($(PREFIX_OP_V2)_TEST_FLEX_SRC)))
$(PREFIX_OP_V2)_TEST_FLEX_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$($(PREFIX_OP_V2)_TEST_FLEX_SRC)))

$(BINDIR)/$(PREFIX_OP_V2)test_parse-dbg: $($(PREFIX_OP_V2)_TEST_PARSE_DBG_OBJ)
	$(LD) -o $@ $^ -lgtest -lpthread -L$(JWT_TOOL)/lib -lncbi-json-dbg -lncbi-secure-dbg -lutf8proc -lmbedcrypto

$(BINDIR)/$(PREFIX_OP_V2)test_parse-rel: $($(PREFIX_OP_V2)_TEST_PARSE_REL_OBJ)
	$(LD) -o $@ $^ -lgtest -lpthread -L$(JWT_TOOL)/lib -lncbi-json-rel -lncbi-secure-rel -lutf8proc -lmbedcrypto

$(BINDIR)/$(PREFIX_OP_V2)test_flex-dbg: $($(PREFIX_OP_V2)_TEST_FLEX_DBG_OBJ)
	$(LD) -o $@ $^ -lgtest -lpthread

$(BINDIR)/$(PREFIX_OP_V2)test_flex-rel: $($(PREFIX_OP_V2)_TEST_FLEX_REL_OBJ)
	$(LD) -o $@ $^ -lgtest -lpthread

run_$(PREFIX_OP_V2)parse_test: run_test_support run_$(PREFIX_OP_V2)flex_test $(BINDIR)/$(PREFIX_OP_V2)test_parse-dbg $(BINDIR)/$(PREFIX_OP_V2)test_parse-rel
	$(BINDIR)/$(PREFIX_OP_V2)test_parse-dbg
	$(BINDIR)/$(PREFIX_OP_V2)test_parse-rel

run_$(PREFIX_OP_V2)flex_test: $(BINDIR)/$(PREFIX_OP_V2)test_flex-dbg $(BINDIR)/$(PREFIX_OP_V2)test_flex-rel
	$(BINDIR)/$(PREFIX_OP_V2)test_flex-dbg
	$(BINDIR)/$(PREFIX_OP_V2)test_flex-rel

.PHONY: op op_prof run_$(PREFIX_OP_V2)test_parse run_$(PREFIX_OP_V2)test_flex

#################################################
# stand alone GCP parser

GCP2JSN_CMN_SRC = \
	$(COMMON_V2_SRC) \
	$(PREFIX_GCP_V2)parser.cpp \
	$(PREFIX_GCP_V2)scanner.cpp \
	GCP_Interface.cpp

GCP2JSN_SRC = $(GCP2JSN_CMN_SRC) gcp2jsn.cpp

GCP2JSN_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$(GCP2JSN_SRC)))
GCP2JSN_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$(GCP2JSN_SRC)))
GCP2JSN_PROF_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.prof.o,$(GCP2JSN_SRC)))

$(OBJDIR)/$(PREFIX_GCP_V2)parser.dbg.o:  $(OBJDIR)/$(PREFIX_GCP_V2)scanner.dbg.o
$(OBJDIR)/$(PREFIX_GCP_V2)parser.rel.o:  $(OBJDIR)/$(PREFIX_GCP_V2)scanner.rel.o
$(OBJDIR)/$(PREFIX_GCP_V2)parser.prof.o:  $(OBJDIR)/$(PREFIX_GCP_V2)scanner.prof.o

$(PREFIX_GCP_V2)scanner.cpp: $(PREFIX_GCP_V2)scanner.l $(PREFIX_GCP_V2)parser.hpp
	flex $(FLEXFLAGS) --header-file=$(PREFIX_GCP_V2)scanner.hpp --outfile=$(PREFIX_GCP_V2)scanner.cpp $(PREFIX_GCP_V2)scanner.l

$(PREFIX_GCP_V2)parser.cpp: $(PREFIX_GCP_V2)parser.y
	bison --report all -d $(PREFIX_GCP_V2)parser.y --output-file $(PREFIX_GCP_V2)parser.cpp

$(BINDIR)/gcp2jsn-dbg: $(GCP2JSN_DBG_OBJ) $(DBG_TOOL_OBJ)
	$(LD) -o $@ $^ -L$(JWT_TOOL)/lib -lncbi-json-dbg -lncbi-secure-dbg -lutf8proc -lmbedcrypto -lpthread

$(BINDIR)/gcp2jsn-rel: $(GCP2JSN_REL_OBJ) $(REL_TOOL_OBJ)
	$(LD) -o $@ $^ -L$(JWT_TOOL)/lib -lncbi-json-rel -lncbi-secure-rel -lutf8proc -lmbedcrypto -lpthread

$(BINDIR)/gcp2jsn-prof: $(GCP2JSN_PROF_OBJ) $(DBG_TOOL_OBJ)
	$(LD) -o $@ $^ $(GPROFFLAGS) -L$(JWT_TOOL)/lib -lncbi-json-rel -lncbi-secure-rel -lutf8proc -lmbedcrypto -lpthread

gcp: mkdir $(BINDIR)/gcp2jsn-dbg $(BINDIR)/gcp2jsn-rel

gcp_prof: mkdir $(BINDIR)/gcp2jsn-prof

$(PREFIX_GCP_V2)_TEST_PARSE_SRC = $(GCP2JSN_CMN_SRC) $(PREFIX_GCP_V2)test_parse.cpp
$(PREFIX_GCP_V2)_TEST_FLEX_SRC = $(PREFIX_GCP_V2)test_flex.cpp $(PREFIX_GCP_V2)scanner.cpp

$(PREFIX_GCP_V2)_TEST_PARSE_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$($(PREFIX_GCP_V2)_TEST_PARSE_SRC)))
$(PREFIX_GCP_V2)_TEST_PARSE_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$($(PREFIX_GCP_V2)_TEST_PARSE_SRC)))

$(PREFIX_GCP_V2)_TEST_FLEX_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$($(PREFIX_GCP_V2)_TEST_FLEX_SRC)))
$(PREFIX_GCP_V2)_TEST_FLEX_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$($(PREFIX_GCP_V2)_TEST_FLEX_SRC)))

$(BINDIR)/$(PREFIX_GCP_V2)test_parse-dbg: $($(PREFIX_GCP_V2)_TEST_PARSE_DBG_OBJ)
	$(LD) -o $@ $^ -lgtest -lpthread -L$(JWT_TOOL)/lib -lncbi-json-dbg -lncbi-secure-dbg -lutf8proc -lmbedcrypto

$(BINDIR)/$(PREFIX_GCP_V2)test_parse-rel: $($(PREFIX_GCP_V2)_TEST_PARSE_REL_OBJ)
	$(LD) -o $@ $^ -lgtest -lpthread -L$(JWT_TOOL)/lib -lncbi-json-rel -lncbi-secure-rel -lutf8proc -lmbedcrypto

$(BINDIR)/$(PREFIX_GCP_V2)test_flex-dbg: $($(PREFIX_GCP_V2)_TEST_FLEX_DBG_OBJ)
	$(LD) -o $@ $^ -lgtest -lpthread

$(BINDIR)/$(PREFIX_GCP_V2)test_flex-rel: $($(PREFIX_GCP_V2)_TEST_FLEX_REL_OBJ)
	$(LD) -o $@ $^ -lgtest -lpthread

run_$(PREFIX_GCP_V2)parse_test: run_test_support run_$(PREFIX_GCP_V2)flex_test $(BINDIR)/$(PREFIX_GCP_V2)test_parse-dbg $(BINDIR)/$(PREFIX_GCP_V2)test_parse-rel
	$(BINDIR)/$(PREFIX_GCP_V2)test_parse-dbg
	$(BINDIR)/$(PREFIX_GCP_V2)test_parse-rel

run_$(PREFIX_GCP_V2)flex_test: $(BINDIR)/$(PREFIX_GCP_V2)test_flex-dbg $(BINDIR)/$(PREFIX_GCP_V2)test_flex-rel
	$(BINDIR)/$(PREFIX_GCP_V2)test_flex-dbg
	$(BINDIR)/$(PREFIX_GCP_V2)test_flex-rel

.PHONY: gcp gcp_prof run_$(PREFIX_GCP_V2)test_parse run_$(PREFIX_GCP_V2)test_flex

#################################################
# stand alone TW parser

TW2JSN_CMN_SRC = \
	$(COMMON_V2_SRC) \
	$(PREFIX_TW_V2)parser.cpp \
	$(PREFIX_TW_V2)scanner.cpp \
	TW_Interface.cpp

TW2JSN_SRC = $(TW2JSN_CMN_SRC) tw2jsn.cpp

TW2JSN_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$(TW2JSN_SRC)))
TW2JSN_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$(TW2JSN_SRC)))
TW2JSN_PROF_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.prof.o,$(TW2JSN_SRC)))

$(OBJDIR)/$(PREFIX_TW_V2)parser.dbg.o:  $(OBJDIR)/$(PREFIX_TW_V2)scanner.dbg.o
$(OBJDIR)/$(PREFIX_TW_V2)parser.rel.o:  $(OBJDIR)/$(PREFIX_TW_V2)scanner.rel.o
$(OBJDIR)/$(PREFIX_TW_V2)parser.prof.o:  $(OBJDIR)/$(PREFIX_TW_V2)scanner.prof.o

$(PREFIX_TW_V2)scanner.cpp: $(PREFIX_TW_V2)scanner.l $(PREFIX_TW_V2)parser.hpp
	flex $(FLEXFLAGS) --header-file=$(PREFIX_TW_V2)scanner.hpp --outfile=$(PREFIX_TW_V2)scanner.cpp $(PREFIX_TW_V2)scanner.l

$(PREFIX_TW_V2)parser.cpp: $(PREFIX_TW_V2)parser.y
	bison --report all -d $(PREFIX_TW_V2)parser.y --output-file $(PREFIX_TW_V2)parser.cpp

$(BINDIR)/tw2jsn-dbg: $(TW2JSN_DBG_OBJ) $(DBG_TOOL_OBJ)
	$(LD) -o $@ $^ -L$(JWT_TOOL)/lib -lncbi-json-dbg -lncbi-secure-dbg -lutf8proc -lmbedcrypto -lpthread

$(BINDIR)/tw2jsn-rel: $(TW2JSN_REL_OBJ) $(REL_TOOL_OBJ)
	$(LD) -o $@ $^ -L$(JWT_TOOL)/lib -lncbi-json-rel -lncbi-secure-rel-no-busy -lutf8proc -lmbedcrypto -lpthread

$(BINDIR)/tw2jsn-prof: $(TW2JSN_PROF_OBJ) $(DBG_TOOL_OBJ)
	$(LD) -o $@ $^ $(GPROFFLAGS) -L$(JWT_TOOL)/lib -lncbi-json-rel -lncbi-secure-rel-no-busy -lutf8proc -lmbedcrypto -lpthread

tw: mkdir $(BINDIR)/tw2jsn-dbg $(BINDIR)/tw2jsn-rel

tw_prof: mkdir $(BINDIR)/tw2jsn-prof

$(PREFIX_TW_V2)_TEST_PARSE_SRC = $(TW2JSN_CMN_SRC) $(PREFIX_TW_V2)test_parse.cpp
$(PREFIX_TW_V2)_TEST_FLEX_SRC = $(PREFIX_TW_V2)test_flex.cpp $(PREFIX_TW_V2)scanner.cpp

$(PREFIX_TW_V2)_TEST_PARSE_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$($(PREFIX_TW_V2)_TEST_PARSE_SRC)))
$(PREFIX_TW_V2)_TEST_PARSE_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$($(PREFIX_TW_V2)_TEST_PARSE_SRC)))

$(PREFIX_TW_V2)_TEST_FLEX_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$($(PREFIX_TW_V2)_TEST_FLEX_SRC)))
$(PREFIX_TW_V2)_TEST_FLEX_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$($(PREFIX_TW_V2)_TEST_FLEX_SRC)))

$(BINDIR)/$(PREFIX_TW_V2)test_parse-dbg: $($(PREFIX_TW_V2)_TEST_PARSE_DBG_OBJ)
	$(LD) -o $@ $^ -lgtest -lpthread -L$(JWT_TOOL)/lib -lncbi-json-dbg -lncbi-secure-dbg -lutf8proc -lmbedcrypto

$(BINDIR)/$(PREFIX_TW_V2)test_parse-rel: $($(PREFIX_TW_V2)_TEST_PARSE_REL_OBJ)
	$(LD) -o $@ $^ -lgtest -lpthread -L$(JWT_TOOL)/lib -lncbi-json-rel -lncbi-secure-rel -lutf8proc -lmbedcrypto

$(BINDIR)/$(PREFIX_TW_V2)test_flex-dbg: $($(PREFIX_TW_V2)_TEST_FLEX_DBG_OBJ)
	$(LD) -o $@ $^ -lgtest -lpthread

$(BINDIR)/$(PREFIX_TW_V2)test_flex-rel: $($(PREFIX_TW_V2)_TEST_FLEX_REL_OBJ)
	$(LD) -o $@ $^ -lgtest -lpthread

run_$(PREFIX_TW_V2)parse_test: run_test_support run_$(PREFIX_TW_V2)flex_test $(BINDIR)/$(PREFIX_TW_V2)test_parse-dbg $(BINDIR)/$(PREFIX_TW_V2)test_parse-rel
	$(BINDIR)/$(PREFIX_TW_V2)test_parse-dbg
	$(BINDIR)/$(PREFIX_TW_V2)test_parse-rel

run_$(PREFIX_TW_V2)flex_test: $(BINDIR)/$(PREFIX_TW_V2)test_flex-dbg $(BINDIR)/$(PREFIX_TW_V2)test_flex-rel
	$(BINDIR)/$(PREFIX_TW_V2)test_flex-dbg
	$(BINDIR)/$(PREFIX_TW_V2)test_flex-rel

.PHONY: tw tw_prof run_$(PREFIX_TW_V2)test_parse run_$(PREFIX_TW_V2)test_flex

#################################################
#################################################
# V2 acceptance ( TODO TW* )

AWS2JSON = $(BINDIR)/aws2jsn-dbg
OP2JSON = $(BINDIR)/op2jsn-dbg
GCP2JSON = $(BINDIR)/gcp2jsn-dbg
TW2JSON = $(BINDIR)/tw2jsn-dbg

v2: aws op gcp tw

jq:
	jq -e . ./expected/*/*.jsonl > /dev/null

test_v2: jq v2 run_$(PREFIX_AWS_V2)parse_test run_$(PREFIX_GCP_V2)parse_test run_$(PREFIX_OP_V2)parse_test run_$(PREFIX_TW_V2)parse_test
	mkdir -p  actual/$(PREFIX_AWS_V2)testlines
	cat data/$(PREFIX_AWS)testlines.log | $(AWS2JSON) actual/$(PREFIX_AWS_V2)testlines/aws >./actual/$(PREFIX_AWS_V2)testlines/stdout 2>./actual/$(PREFIX_AWS_V2)testlines/stderr \
						&& diff ./expected/$(PREFIX_AWS_V2)testlines ./actual/$(PREFIX_AWS_V2)testlines
	@ echo "test_aws v2 passed"
	@ rm -rf actual/$(PREFIX_AWS_V2)*
	#
	mkdir -p  actual/$(PREFIX_OP_V2)testlines
	cat data/$(PREFIX_OP)testlines.log | $(OP2JSON) actual/$(PREFIX_OP_V2)testlines/op >./actual/$(PREFIX_OP_V2)testlines/stdout 2>./actual/$(PREFIX_OP_V2)testlines/stderr \
						&& diff ./expected/$(PREFIX_OP_V2)testlines ./actual/$(PREFIX_OP_V2)testlines
	@ echo "test_op v2 passed"
	@ rm -rf actual/$(PREFIX_OP_V2)*
	#
	mkdir -p  actual/$(PREFIX_GCP_V2)testlines
	cat data/$(PREFIX_GCP)testlines.log | $(GCP2JSON) actual/$(PREFIX_GCP_V2)testlines/gcp >./actual/$(PREFIX_GCP_V2)testlines/stdout 2>./actual/$(PREFIX_GCP_V2)testlines/stderr \
						&& diff ./expected/$(PREFIX_GCP_V2)testlines ./actual/$(PREFIX_GCP_V2)testlines
	@ echo "test_gcp v2 passed"
	@ rm -rf actual/$(PREFIX_GCP_V2)*
	#
	mkdir -p  actual/$(PREFIX_TW_V2)testlines
	cat data/$(PREFIX_TW)testlines.log | $(TW2JSON) actual/$(PREFIX_TW_V2)testlines/tw >./actual/$(PREFIX_TW_V2)testlines/stdout 2>./actual/$(PREFIX_TW_V2)testlines/stderr \
						&& diff ./expected/$(PREFIX_TW_V2)testlines ./actual/$(PREFIX_TW_V2)testlines
	@ echo "test_tw v2 passed"
	@ rm -rf actual/$(PREFIX_TW_V2)*

.PHONY: v2 test_v2 jq

#################################################
# some valgrind checks...

VG_OPTIONS := --leak-check=full --error-exitcode=1

# which version to run under valginrd (uncomment one)
BUILD = dbg
#BUILD = rel

vg_op : $(BINDIR)/op2jsn-$(BUILD) $(BINDIR)/$(PREFIX_OP_V2)test_flex-$(BUILD) $(BINDIR)/op_test_parse-$(BUILD)
	valgrind $(VG_OPTIONS) $(BINDIR)/$(PREFIX_OP_V2)test_flex-$(BUILD)
	valgrind $(VG_OPTIONS) $(BINDIR)/$(PREFIX_OP_V2)test_parse-$(BUILD)
	cat data/$(PREFIX_OP)testlines.log | valgrind $(VG_OPTIONS) $(BINDIR)/op2jsn-$(BUILD) actual/op

vg_gcp : $(BINDIR)/gcp2jsn-$(BUILD) $(BINDIR)/$(PREFIX_GCP_V2)test_flex-$(BUILD) $(BINDIR)/$(PREFIX_GCP_V2)test_parse-$(BUILD)
	valgrind $(VG_OPTIONS) $(BINDIR)/$(PREFIX_GCP_V2)test_flex-$(BUILD)
	valgrind $(VG_OPTIONS) $(BINDIR)/$(PREFIX_GCP_V2)test_parse-$(BUILD)
	cat data/$(PREFIX_GCP)testlines.log | valgrind $(VG_OPTIONS) $(BINDIR)/gcp2jsn-$(BUILD) actual/gcp

vg_aws : $(BINDIR)/aws2jsn-$(BUILD) $(BINDIR)/$(PREFIX_AWS_V2)test_flex-$(BUILD) $(BINDIR)/$(PREFIX_AWS_V2)test_parse-$(BUILD)
	valgrind $(VG_OPTIONS) $(BINDIR)/$(PREFIX_AWS_V2)test_flex-$(BUILD)
	valgrind $(VG_OPTIONS) $(BINDIR)/$(PREFIX_AWS_V2)test_parse-$(BUILD)
	cat data/$(PREFIX_AWS)testlines.log | valgrind $(VG_OPTIONS) $(BINDIR)/aws2jsn-$(BUILD) actual/aws

vg_all : vg_op vg_gcp vg_aws

.PHONY: vg_op vg_gcp vg_aws vg_all
