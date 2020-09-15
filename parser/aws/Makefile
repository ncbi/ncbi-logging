BINDIR = ../bin
OBJDIR = ../obj

CC := gcc -c
CPP := g++ -c
LD := g++
CXXFLAGS += -I . -I .. -I ../common/ -MMD
GPROFFLAGS = -pg -O3
FLEXFLAGS = -8 --align

PREFIX_AWS := aws_

all: cmn aws
	@echo "done"

cmn:
	$(MAKE) -C ../common

test: acc_test

clean: clean_aws
	rm -rf *.output

mkdir:
	@ mkdir -p $(BINDIR) $(OBJDIR)

#################################################
# generic rules for compiling
$(OBJDIR)/%.dbg.o: %.cpp
	$(CPP) $(CXXFLAGS) -g $< -o $(OBJDIR)/$(*F).dbg.o

$(OBJDIR)/%.rel.o: %.cpp
	$(CPP) $(CXXFLAGS) -O3 $< -o $(OBJDIR)/$(*F).rel.o

$(OBJDIR)/%.prof.o: %.cpp
	$(CPP) $(CXXFLAGS) $(GPROFFLAGS) $< -o $(OBJDIR)/$(*F).prof.o

#################################################
# generic rules for flex and bison
%scanner.cpp: %scanner.l %parser.hpp %parser.cpp
	flex $(FLEXFLAGS) --header-file=$(subst .cpp,.hpp,$@) --outfile=$@ $<

%parser.cpp : %parser.y
	bison --report all -d $< --output-file $@

-include $(OBJDIR)/*.d

.phony: all test clean mkdir

#################################################
# Specify location of ncbi-vdb3/jwt-tool in JWT_TOOL if different
JWT_TOOL ?= ../../../ncbi-vdb3/jwt-tool

CXXFLAGS += -I$(JWT_TOOL)/inc -I$(JWT_TOOL)/utf8proc -I$(JWT_TOOL)/tool

LDFLAGS = -L$(JWT_TOOL)/lib -lncbi-json-dbg -lncbi-secure-dbg -lutf8proc -lmbedcrypto -lpthread

#################################################

# NOTE: keep matching the set of source files in common/
COMMON_SRC = \
	CatWriters.cpp \
	Formatters.cpp \
	ReceiverInterface.cpp \
	Queues.cpp \
	LineSplitters.cpp \
	Tool.cpp \
	cmdline.cpp

COMMON_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$(COMMON_SRC)))
COMMON_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$(COMMON_SRC)))
COMMON_PROF_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.prof.o,$(COMMON_SRC)))

#################################################
# stand alone AWS parser

AWS2JSN_CMN_SRC = \
	$(PREFIX_AWS)parser.cpp \
	$(PREFIX_AWS)scanner.cpp \
	AWS_Interface.cpp

AWS2JSN_SRC = $(AWS2JSN_CMN_SRC) aws2jsn.cpp

AWS2JSN_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$(AWS2JSN_SRC)))
AWS2JSN_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$(AWS2JSN_SRC)))
AWS2JSN_PROF_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.prof.o,$(AWS2JSN_SRC)))

$(OBJDIR)/$(PREFIX_AWS)parser.dbg.o:  	$(OBJDIR)/$(PREFIX_AWS)scanner.dbg.o
$(OBJDIR)/$(PREFIX_AWS)parser.rel.o:  	$(OBJDIR)/$(PREFIX_AWS)scanner.rel.o
$(OBJDIR)/$(PREFIX_AWS)parser.prof.o:  	$(OBJDIR)/$(PREFIX_AWS)scanner.prof.o

$(OBJDIR)/AWS_Interface.dbg.o $(OBJDIR)/AWS_Interface.rel.o $(OBJDIR)/AWS_Interface.prof.o : $(PREFIX_AWS)parser.hpp
$(PREFIX_AWS)parser.hpp : $(PREFIX_AWS)parser.cpp

$(BINDIR)/aws2jsn-dbg: $(AWS2JSN_DBG_OBJ) $(DBG_TOOL_OBJ)
	$(LD) -o $@ $^ $(COMMON_DBG_OBJ) $(LDFLAGS)

$(BINDIR)/aws2jsn-rel: $(AWS2JSN_REL_OBJ) $(REL_TOOL_OBJ)
	$(LD) -o $@ $^ $(COMMON_REL_OBJ) $(LDFLAGS)

$(BINDIR)/aws2jsn-prof: $(AWS2JSN_PROF_OBJ) $(PROF_TOOL_OBJ)
	$(LD) -o $@ $^ $(COMMON_PROF_OBJ) $(GPROFFLAGS) $(LDFLAGS)

aws: mkdir $(BINDIR)/aws2jsn-dbg $(BINDIR)/aws2jsn-rel

aws_prof: mkdir $(BINDIR)/aws2jsn-prof
	#TODO: revive

$(PREFIX_AWS)_TEST_PARSE_SRC = $(AWS2JSN_CMN_SRC) $(PREFIX_AWS)test_parse.cpp
$(PREFIX_AWS)_TEST_FLEX_SRC = $(PREFIX_AWS)test_flex.cpp $(PREFIX_AWS)scanner.cpp

$(PREFIX_AWS)_TEST_PARSE_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$($(PREFIX_AWS)_TEST_PARSE_SRC)))
$(PREFIX_AWS)_TEST_PARSE_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$($(PREFIX_AWS)_TEST_PARSE_SRC)))

$(PREFIX_AWS)_TEST_FLEX_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$($(PREFIX_AWS)_TEST_FLEX_SRC)))
$(PREFIX_AWS)_TEST_FLEX_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$($(PREFIX_AWS)_TEST_FLEX_SRC)))

$(BINDIR)/$(PREFIX_AWS)test_parse-dbg: $($(PREFIX_AWS)_TEST_PARSE_DBG_OBJ)
	$(LD) -o $@ $^  $(COMMON_DBG_OBJ) -lgtest $(LDFLAGS)

$(BINDIR)/$(PREFIX_AWS)test_parse-rel: $($(PREFIX_AWS)_TEST_PARSE_REL_OBJ)
	$(LD) -o $@ $^  $(COMMON_REL_OBJ) -lgtest $(LDFLAGS)

$(BINDIR)/$(PREFIX_AWS)test_flex-dbg: $($(PREFIX_AWS)_TEST_FLEX_DBG_OBJ)
	$(LD) -o $@ $^ -lgtest -lpthread

$(BINDIR)/$(PREFIX_AWS)test_flex-rel: $($(PREFIX_AWS)_TEST_FLEX_REL_OBJ)
	$(LD) -o $@ $^ -lgtest -lpthread

run_$(PREFIX_AWS)parse_test: aws run_$(PREFIX_AWS)flex_test $(BINDIR)/$(PREFIX_AWS)test_parse-dbg $(BINDIR)/$(PREFIX_AWS)test_parse-rel
	$(BINDIR)/$(PREFIX_AWS)test_parse-dbg
	$(BINDIR)/$(PREFIX_AWS)test_parse-rel

run_$(PREFIX_AWS)flex_test: aws $(BINDIR)/$(PREFIX_AWS)test_flex-dbg $(BINDIR)/$(PREFIX_AWS)test_flex-rel
	$(BINDIR)/$(PREFIX_AWS)test_flex-dbg
	$(BINDIR)/$(PREFIX_AWS)test_flex-rel

clean_aws:
	rm -rf $(OBJDIR)/aws* $(OBJDIR)/AWS_* $(BINDIR)/aws* ./$(PREFIX_AWS)parser.?pp ./$(PREFIX_AWS)scanner.?pp

.PHONY: aws aws_prof clean_aws run_$(PREFIX_AWS)parse_test run_$(PREFIX_AWS)flex_test

#################################################
# acceptance

AWS2JSON = $(BINDIR)/aws2jsn-dbg

acc_test: all run_$(PREFIX_AWS)parse_test
	mkdir -p  actual/$(PREFIX_AWS)testlines
	cat ../data/aws_testlines.log | $(AWS2JSON) actual/$(PREFIX_AWS)testlines/aws >./actual/$(PREFIX_AWS)testlines/stdout 2>./actual/$(PREFIX_AWS)testlines/stderr \
						&& diff ../expected/$(PREFIX_AWS)testlines ./actual/$(PREFIX_AWS)testlines
	@ echo "test_aws passed"
	@ rm -rf actual/$(PREFIX_AWS)*
	@echo "done"

.PHONY: acc_test

#################################################
# valgrind

VG_OPTIONS := --leak-check=full --error-exitcode=1

# which version to run under valginrd (uncomment one)
BUILD = dbg
#BUILD = rel

vg : $(BINDIR)/aws2jsn-$(BUILD) $(BINDIR)/$(PREFIX_AWS)test_flex-$(BUILD) $(BINDIR)/$(PREFIX_AWS)test_parse-$(BUILD)
	valgrind $(VG_OPTIONS) $(BINDIR)/$(PREFIX_AWS)test_flex-$(BUILD)
	valgrind $(VG_OPTIONS) $(BINDIR)/$(PREFIX_AWS)test_parse-$(BUILD)
	cat data/aws_testlines.log | valgrind $(VG_OPTIONS) $(BINDIR)/aws2jsn-$(BUILD) actual/aws

.PHONY: vg

#-------------------------------------------------------------------------------
# fuzz testing

FUZZ_THREADS = 2
FUZZ_RUNS ?= 5000

FUZZ_OPT = -DTHREAD_NUM=$(FUZZ_THREADS) -fsanitize=fuzzer,address,signed-integer-overflow -fprofile-instr-generate -fcoverage-mapping

$(OBJDIR)/%.fuzz.o: %.cpp
	clang++ -c $< -o $@ -g $(CXXFLAGS) $(FUZZ_OPT)

clean_fuzz:
	rm -rf fuzz/

.phony: clean_fuzz run_fuzz

AWS_FUZZ_SRC = \
	$(AWS2JSN_CMN_SRC) \
	$(PREFIX_AWS)fuzz.cpp

AWS_FUZZ_OBJ = \
	$(addprefix $(OBJDIR)/,$(subst .cpp,.fuzz.o,$(AWS_FUZZ_SRC)))

$(BINDIR)/aws-fuzz: aws $(AWS_FUZZ_OBJ)
	clang++ -g -o $@ $(AWS_FUZZ_OBJ) $(FUZZ_OPT) $(COMMON_DBG_OBJ) $(LDFLAGS)

#AWS_FUZZ_DICT ?= -dict=fuzz/aws-dict

run_fuzz: $(BINDIR)/aws-fuzz
	 mkdir -p fuzz/aws-corpus fuzz/aws-seeds
	 cp data/aws_testlines.log fuzz/aws-seeds/
	 $^ $(AWS_FUZZ_DICT) -runs=$(FUZZ_RUNS) fuzz/aws-corpus fuzz/aws-seeds

.phony: run_aws_fuzz
