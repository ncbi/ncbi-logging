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
PREFIX_AWS := aws_
PREFIX_GCP := gcp_

all: log2jsn_
	@echo "done"

prof: log2jsn_prof

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

CXXFLAGS += -I$(JWT_TOOL)/inc -I$(JWT_TOOL)/tool

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
	cat data/op_unrecognized.log    | $(LOG2JSON) op -p >./actual/op_unrecognized.out 2>./actual/op_unrecognized.err \
						&& diff ./expected/op_unrecognized.out ./actual/op_unrecognized.out \
						&& diff ./expected/op_unrecognized.err ./actual/op_unrecognized.err 
	cat data/aws_unrecognized.log | $(LOG2JSON) aws >./actual/aws_unrecognized.out 2>./actual/aws_unrecognized.err \
						&& diff ./expected/aws_unrecognized.out ./actual/aws_unrecognized.out \
						&& diff ./expected/aws_unrecognized.err ./actual/aws_unrecognized.err
	cat data/gcp_unrecognized.log | $(LOG2JSON) gcp >./actual/gcp_unrecognized.out 2>./actual/gcp_unrecognized.err \
						&& diff ./expected/gcp_unrecognized.out ./actual/gcp_unrecognized.out \
						&& diff ./expected/gcp_unrecognized.err ./actual/gcp_unrecognized.err
	cat data/op_unrecognized.log  | $(LOG2JSON) op -p -j --readable >./actual/op_unrecognized_pretty.out 2>./actual/op_unrecognized.err \
						&& diff ./expected/op_unrecognized_pretty.out ./actual/op_unrecognized_pretty.out \
						&& diff ./expected/op_unrecognized.err ./actual/op_unrecognized.err 
	cat data/op_unrecognized.log | $(LOG2JSON) op -l2 > ./actual/op_filtered.out 2>./actual/op_filtered.err \
						&& diff ./expected/op_filtered.out ./actual/op_filtered.out \
						&& diff ./expected/op_filtered.err ./actual/op_filtered.err 
	cat data/aws_unrecognized.log | $(LOG2JSON) aws -l2 > ./actual/aws_filtered.out 2>./actual/aws_filtered.err \
						&& diff ./expected/aws_filtered.out ./actual/aws_filtered.out \
						&& diff ./expected/aws_filtered.err ./actual/aws_filtered.err 
	cat data/gcp_unrecognized.log | $(LOG2JSON) gcp -l2 > ./actual/gcp_filtered.out 2>./actual/gcp_filtered.err \
						&& diff ./expected/gcp_filtered.out ./actual/gcp_filtered.out \
						&& diff ./expected/gcp_filtered.err ./actual/gcp_filtered.err 

	@ echo "test2json passed"
	@ rm -rf actual/run2jsn*

.PHONY: log2jsn_ log2jsn_prof test2jsn

#################################################
# stand alone AWS parser

AWS2JSN_SRC = aws2jsn.cpp helper.cpp

AWS2JSN_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$(AWS2JSN_SRC)))
AWS2JSN_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$(AWS2JSN_SRC)))
AWS2JSN_PROF_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.prof.o,$(AWS2JSN_SRC)))

$(BINDIR)/aws2jsn-dbg: $(AWS2JSN_DBG_OBJ) $(DBG_CMD_OBJ)
	$(LD) -o $@ $^ -L$(JWT_TOOL)/lib -lncbi-secure-dbg -lutf8proc -lmbedcrypto

$(BINDIR)/aws2jsn-rel: $(AWS2JSN_REL_OBJ) $(REL_CMD_OBJ) 
	$(LD) -o $@ $^ -L$(JWT_TOOL)/lib -lncbi-secure-rel -lutf8proc -lmbedcrypto

$(BINDIR)/aws2jsn-prof: $(AWS2JSN_PROF_OBJ) $(DBG_CMD_OBJ)
	$(LD) -o $@ $^ $(GPROFFLAGS) -L$(JWT_TOOL)/lib -lncbi-secure-prof -lutf8proc -lmbedcrypto

aws: mkdir $(BINDIR)/aws2jsn-dbg $(BINDIR)/aws2jsn-rel

aws_prof: mkdir $(BINDIR)/log2jsn-prof

AWS_TEST_IMPL_SRC = \
	$(PREFIX_AWS)test_impl.cpp helper.cpp

AWS_TEST_IMPL_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$(AWS_TEST_IMPL_SRC)))
AWS_TEST_IMPL_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$(AWS_TEST_IMPL_SRC)))

$(BINDIR)/$(PREFIX_AWS)test_impl-dbg: $(AWS_TEST_IMPL_DBG_OBJ)
	$(LD) -o $@ $^ -lgtest -lpthread

$(BINDIR)/$(PREFIX_AWS)test_impl-rel: $(AWS_TEST_IMPL_REL_OBJ)
	$(LD) -o $@ $^ -lgtest -lpthread 

run_$(PREFIX_AWS)impltest: $(BINDIR)/$(PREFIX_AWS)test_impl-dbg $(BINDIR)/$(PREFIX_AWS)test_impl-rel
	$(BINDIR)/$(PREFIX_AWS)test_impl-dbg
	$(BINDIR)/$(PREFIX_AWS)test_impl-rel

.PHONY: aws aws_prof run_$(PREFIX_AWS)test_impl 

#################################################
#################################################
# acceptance 

# which version to run for acceptance (uncomment one)
AWS2JSON = $(BINDIR)/aws2jsn-dbg
#AWS2JSON = $(BINDIR)/aws2jsn-rel

test_aws: aws
	@ mkdir -p actual
	@ echo "test_aws passed"
	@ rm -rf actual/aws2jsn*

.PHONY: aws aws2jsn_prof test_aws

#################################################
# some valgrind checks...

VG_OPTIONS := --leak-check=full --error-exitcode=1

# which version to run under valginrd (uncomment one)
BUILD = dbg
#BUILD = rel

vg_op : $(BINDIR)/log2jsn-$(BUILD) $(BINDIR)/op_test_flex-$(BUILD) $(BINDIR)/op_test_parse-$(BUILD)
	valgrind $(VG_OPTIONS) $(BINDIR)/op_test_flex-$(BUILD)
	valgrind $(VG_OPTIONS) $(BINDIR)/op_test_parse-$(BUILD)
	cat data/op_two_events.txt | valgrind $(VG_OPTIONS) $(BINDIR)/log2jsn-$(BUILD) op

vg_gcp : $(BINDIR)/log2jsn-$(BUILD) $(BINDIR)/gcp_test_flex-$(BUILD) $(BINDIR)/gcp_test_parse-$(BUILD)
	valgrind $(VG_OPTIONS) $(BINDIR)/gcp_test_flex-$(BUILD)
	valgrind $(VG_OPTIONS) $(BINDIR)/gcp_test_parse-$(BUILD)
	cat data/gcp_unrecognized.log | valgrind $(VG_OPTIONS) $(BINDIR)/log2jsn-$(BUILD) gcp

vg_aws : $(BINDIR)/log2jsn-$(BUILD) $(BINDIR)/aws_test_flex-$(BUILD) $(BINDIR)/aws_test_parse-$(BUILD)
	valgrind $(VG_OPTIONS) $(BINDIR)/aws_test_flex-$(BUILD)
	valgrind $(VG_OPTIONS) $(BINDIR)/aws_test_parse-$(BUILD)
	cat data/aws_unrecognized.log | valgrind $(VG_OPTIONS) $(BINDIR)/log2jsn-$(BUILD) aws

vg_all : vg_op vg_gcp vg_aws

.PHONY: vg_op vg_gcp vg_aws vg_all
