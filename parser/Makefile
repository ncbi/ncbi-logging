BINDIR = bin
OBJDIR = obj

CC := gcc -c
CPP := g++ -c
LD := g++
CXXFLAGS += -I . -I common/ -MMD
GPROFFLAGS = -pg -O3
FLEXFLAGS = -8 --align

PREFIX_OP := op_
PREFIX_GCP := gcp_
PREFIX_TW := tw_

all: cmn aws op gcp tw
	@echo "done"

cmn:
	$(MAKE) -C common

test: test_cmn acc_test

test_cmn:
	$(MAKE) -C common test

#do not rm XXX_parser.*/XXX_scanner.*! this would deleted *.l and *.y too!
clean: aws_clean op_clean gcp_clean tw_clean
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
JWT_TOOL ?= ../../ncbi-vdb3/jwt-tool

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

aws: mkdir
	$(MAKE) -C aws

aws_prof:
	$(MAKE) -C aws prof

aws_test:
	$(MAKE) -C aws test

aws_clean:
	$(MAKE) -C aws clean

.PHONY: aws aws_prof aws_clean aws_test

#################################################
# stand alone OP parser

OP2JSN_CMN_SRC = \
	$(PREFIX_OP)parser.cpp \
	$(PREFIX_OP)scanner.cpp \
	OP_Interface.cpp

OP2JSN_SRC = $(OP2JSN_CMN_SRC) op2jsn.cpp

OP2JSN_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$(OP2JSN_SRC)))
OP2JSN_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$(OP2JSN_SRC)))
OP2JSN_PROF_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.prof.o,$(OP2JSN_SRC)))

$(OBJDIR)/$(PREFIX_OP)parser.dbg.o:  $(OBJDIR)/$(PREFIX_OP)scanner.dbg.o
$(OBJDIR)/$(PREFIX_OP)parser.rel.o:  $(OBJDIR)/$(PREFIX_OP)scanner.rel.o
$(OBJDIR)/$(PREFIX_OP)parser.prof.o:  $(OBJDIR)/$(PREFIX_OP)scanner.prof.o

$(OBJDIR)/OP_Interface.dbg.o $(OBJDIR)/OP_Interface.rel.o $(OBJDIR)/OP_Interface.prof.o : $(PREFIX_OP)parser.hpp
$(PREFIX_OP)parser.hpp : $(PREFIX_OP)parser.cpp

$(BINDIR)/op2jsn-dbg: $(OP2JSN_DBG_OBJ) $(DBG_TOOL_OBJ)
	$(LD) -o $@ $^ $(COMMON_DBG_OBJ) $(LDFLAGS)

$(BINDIR)/op2jsn-rel: $(OP2JSN_REL_OBJ) $(REL_TOOL_OBJ)
	$(LD) -o $@ $^ $(COMMON_DBG_OBJ) $(LDFLAGS)

$(BINDIR)/op2jsn-prof: $(OP2JSN_PROF_OBJ) $(DBG_TOOL_OBJ)
	$(LD) -o $@ $^ $(GPROFFLAGS) -L$(JWT_TOOL)/lib -lncbi-json-rel -lncbi-secure-rel $(LDFLAGS)

op: mkdir $(BINDIR)/op2jsn-dbg $(BINDIR)/op2jsn-rel

op_prof: mkdir $(BINDIR)/op2jsn-prof
	#TBD

$(PREFIX_OP)_TEST_PARSE_SRC = $(OP2JSN_CMN_SRC) $(PREFIX_OP)test_parse.cpp
$(PREFIX_OP)_TEST_FLEX_SRC = $(PREFIX_OP)test_flex.cpp $(PREFIX_OP)scanner.cpp

$(PREFIX_OP)_TEST_PARSE_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$($(PREFIX_OP)_TEST_PARSE_SRC)))
$(PREFIX_OP)_TEST_PARSE_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$($(PREFIX_OP)_TEST_PARSE_SRC)))

$(PREFIX_OP)_TEST_FLEX_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$($(PREFIX_OP)_TEST_FLEX_SRC)))
$(PREFIX_OP)_TEST_FLEX_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$($(PREFIX_OP)_TEST_FLEX_SRC)))

$(BINDIR)/$(PREFIX_OP)test_parse-dbg: $($(PREFIX_OP)_TEST_PARSE_DBG_OBJ)
	$(LD) -o $@ $^ -lgtest $(COMMON_DBG_OBJ) $(LDFLAGS)

$(BINDIR)/$(PREFIX_OP)test_parse-rel: $($(PREFIX_OP)_TEST_PARSE_REL_OBJ)
	$(LD) -o $@ $^ -lgtest $(COMMON_DBG_OBJ) $(LDFLAGS)

$(BINDIR)/$(PREFIX_OP)test_flex-dbg: $($(PREFIX_OP)_TEST_FLEX_DBG_OBJ)
	$(LD) -o $@ $^ -lgtest -lpthread

$(BINDIR)/$(PREFIX_OP)test_flex-rel: $($(PREFIX_OP)_TEST_FLEX_REL_OBJ)
	$(LD) -o $@ $^ -lgtest -lpthread

run_$(PREFIX_OP)parse_test: op run_$(PREFIX_OP)flex_test $(BINDIR)/$(PREFIX_OP)test_parse-dbg $(BINDIR)/$(PREFIX_OP)test_parse-rel
	$(BINDIR)/$(PREFIX_OP)test_parse-dbg
	$(BINDIR)/$(PREFIX_OP)test_parse-rel

run_$(PREFIX_OP)flex_test: op $(BINDIR)/$(PREFIX_OP)test_flex-dbg $(BINDIR)/$(PREFIX_OP)test_flex-rel
	$(BINDIR)/$(PREFIX_OP)test_flex-dbg
	$(BINDIR)/$(PREFIX_OP)test_flex-rel

op_clean:
	rm -rf $(OBJDIR)/op* $(OBJDIR)/OP_* $(BINDIR)/op* ./$(PREFIX_OP)parser.?pp ./$(PREFIX_OP)scanner.?pp

.PHONY: op op_prof op_clean run_$(PREFIX_OP)parse_test run_$(PREFIX_OP)flex_test

#################################################
# stand alone GCP parser

GCP2JSN_CMN_SRC = \
	$(PREFIX_GCP)parser.cpp \
	$(PREFIX_GCP)scanner.cpp \
	GCP_Interface.cpp

GCP2JSN_SRC = $(GCP2JSN_CMN_SRC) gcp2jsn.cpp

GCP2JSN_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$(GCP2JSN_SRC)))
GCP2JSN_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$(GCP2JSN_SRC)))
GCP2JSN_PROF_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.prof.o,$(GCP2JSN_SRC)))

$(OBJDIR)/$(PREFIX_GCP)parser.dbg.o:  $(OBJDIR)/$(PREFIX_GCP)scanner.dbg.o
$(OBJDIR)/$(PREFIX_GCP)parser.rel.o:  $(OBJDIR)/$(PREFIX_GCP)scanner.rel.o
$(OBJDIR)/$(PREFIX_GCP)parser.prof.o:  $(OBJDIR)/$(PREFIX_GCP)scanner.prof.o

$(OBJDIR)/GCP_Interface.dbg.o $(OBJDIR)/GCP_Interface.rel.o $(OBJDIR)/GCP_Interface.prof.o : $(PREFIX_GCP)parser.hpp
$(PREFIX_GCP)parser.hpp : $(PREFIX_GCP)parser.cpp

$(BINDIR)/gcp2jsn-dbg: $(GCP2JSN_DBG_OBJ) $(DBG_TOOL_OBJ)
	$(LD) -o $@ $^  $(COMMON_DBG_OBJ) $(LDFLAGS)

$(BINDIR)/gcp2jsn-rel: $(GCP2JSN_REL_OBJ) $(REL_TOOL_OBJ)
	$(LD) -o $@ $^  $(COMMON_DBG_OBJ) $(LDFLAGS)

$(BINDIR)/gcp2jsn-prof: $(GCP2JSN_PROF_OBJ) $(DBG_TOOL_OBJ)
	$(LD) -o $@ $^ $(GPROFFLAGS)  $(COMMON_DBG_OBJ) $(LDFLAGS)

gcp: mkdir $(BINDIR)/gcp2jsn-dbg $(BINDIR)/gcp2jsn-rel

gcp_prof: mkdir $(BINDIR)/gcp2jsn-prof
	#TBD

$(PREFIX_GCP)_TEST_PARSE_SRC = $(GCP2JSN_CMN_SRC) $(PREFIX_GCP)test_parse.cpp
$(PREFIX_GCP)_TEST_FLEX_SRC = $(PREFIX_GCP)test_flex.cpp $(PREFIX_GCP)scanner.cpp

$(PREFIX_GCP)_TEST_PARSE_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$($(PREFIX_GCP)_TEST_PARSE_SRC)))
$(PREFIX_GCP)_TEST_PARSE_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$($(PREFIX_GCP)_TEST_PARSE_SRC)))

$(PREFIX_GCP)_TEST_FLEX_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$($(PREFIX_GCP)_TEST_FLEX_SRC)))
$(PREFIX_GCP)_TEST_FLEX_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$($(PREFIX_GCP)_TEST_FLEX_SRC)))

$(BINDIR)/$(PREFIX_GCP)test_parse-dbg: $($(PREFIX_GCP)_TEST_PARSE_DBG_OBJ)
	$(LD) -o $@ $^ -lgtest  $(COMMON_DBG_OBJ) $(LDFLAGS)

$(BINDIR)/$(PREFIX_GCP)test_parse-rel: $($(PREFIX_GCP)_TEST_PARSE_REL_OBJ)
	$(LD) -o $@ $^ -lgtest  $(COMMON_DBG_OBJ) $(LDFLAGS)

$(BINDIR)/$(PREFIX_GCP)test_flex-dbg: $($(PREFIX_GCP)_TEST_FLEX_DBG_OBJ)
	$(LD) -o $@ $^ -lgtest -lpthread

$(BINDIR)/$(PREFIX_GCP)test_flex-rel: $($(PREFIX_GCP)_TEST_FLEX_REL_OBJ)
	$(LD) -o $@ $^ -lgtest -lpthread

run_$(PREFIX_GCP)parse_test: gcp run_$(PREFIX_GCP)flex_test $(BINDIR)/$(PREFIX_GCP)test_parse-dbg $(BINDIR)/$(PREFIX_GCP)test_parse-rel
	$(BINDIR)/$(PREFIX_GCP)test_parse-dbg
	$(BINDIR)/$(PREFIX_GCP)test_parse-rel

run_$(PREFIX_GCP)flex_test: gcp $(BINDIR)/$(PREFIX_GCP)test_flex-dbg $(BINDIR)/$(PREFIX_GCP)test_flex-rel
	$(BINDIR)/$(PREFIX_GCP)test_flex-dbg
	$(BINDIR)/$(PREFIX_GCP)test_flex-rel

gcp_clean:
	rm -rf $(OBJDIR)/gcp* $(OBJDIR)/GCP_* $(BINDIR)/gcp* ./$(PREFIX_GCP)parser.?pp ./$(PREFIX_GCP)scanner.?pp

.PHONY: gcp gcp_prof gcp_clean run_$(PREFIX_GCP)parse_test run_$(PREFIX_GCP)flex_test

#################################################
# stand alone TW parser

TW2JSN_CMN_SRC = \
	$(PREFIX_TW)parser.cpp \
	$(PREFIX_TW)scanner.cpp \
	TW_Interface.cpp

TW2JSN_SRC = $(TW2JSN_CMN_SRC) tw2jsn.cpp

TW2JSN_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$(TW2JSN_SRC)))
TW2JSN_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$(TW2JSN_SRC)))
TW2JSN_PROF_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.prof.o,$(TW2JSN_SRC)))

$(OBJDIR)/$(PREFIX_TW)parser.dbg.o:  $(OBJDIR)/$(PREFIX_TW)scanner.dbg.o
$(OBJDIR)/$(PREFIX_TW)parser.rel.o:  $(OBJDIR)/$(PREFIX_TW)scanner.rel.o
$(OBJDIR)/$(PREFIX_TW)parser.prof.o:  $(OBJDIR)/$(PREFIX_TW)scanner.prof.o

$(OBJDIR)/TW_Interface.dbg.o $(OBJDIR)/TW_Interface.rel.o $(OBJDIR)/TW_Interface.prof.o : $(PREFIX_TW)parser.hpp
$(PREFIX_TW)parser.hpp : $(PREFIX_TW)parser.cpp

$(BINDIR)/tw2jsn-dbg: $(TW2JSN_DBG_OBJ) $(DBG_TOOL_OBJ)
	$(LD) -o $@ $^ $(COMMON_DBG_OBJ) $(LDFLAGS)

$(BINDIR)/tw2jsn-rel: $(TW2JSN_REL_OBJ) $(REL_TOOL_OBJ)
	$(LD) -o $@ $^ $(COMMON_DBG_OBJ) $(LDFLAGS)

$(BINDIR)/tw2jsn-prof: $(TW2JSN_PROF_OBJ) $(DBG_TOOL_OBJ)
	$(LD) -o $@ $^ $(GPROFFLAGS) $(COMMON_DBG_OBJ) $(LDFLAGS)

tw: mkdir $(BINDIR)/tw2jsn-dbg $(BINDIR)/tw2jsn-rel

tw_prof: mkdir $(BINDIR)/tw2jsn-prof
	#TBD

$(PREFIX_TW)_TEST_PARSE_SRC = $(TW2JSN_CMN_SRC) $(PREFIX_TW)test_parse.cpp
$(PREFIX_TW)_TEST_FLEX_SRC = $(PREFIX_TW)test_flex.cpp $(PREFIX_TW)scanner.cpp

$(PREFIX_TW)_TEST_PARSE_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$($(PREFIX_TW)_TEST_PARSE_SRC)))
$(PREFIX_TW)_TEST_PARSE_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$($(PREFIX_TW)_TEST_PARSE_SRC)))

$(PREFIX_TW)_TEST_FLEX_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$($(PREFIX_TW)_TEST_FLEX_SRC)))
$(PREFIX_TW)_TEST_FLEX_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$($(PREFIX_TW)_TEST_FLEX_SRC)))

$(BINDIR)/$(PREFIX_TW)test_parse-dbg: $($(PREFIX_TW)_TEST_PARSE_DBG_OBJ)
	$(LD) -o $@ $^ -lgtest $(COMMON_DBG_OBJ) $(LDFLAGS)

$(BINDIR)/$(PREFIX_TW)test_parse-rel: $($(PREFIX_TW)_TEST_PARSE_REL_OBJ)
	$(LD) -o $@ $^ -lgtest $(COMMON_DBG_OBJ) $(LDFLAGS)

$(BINDIR)/$(PREFIX_TW)test_flex-dbg: $($(PREFIX_TW)_TEST_FLEX_DBG_OBJ)
	$(LD) -o $@ $^ -lgtest -lpthread

$(BINDIR)/$(PREFIX_TW)test_flex-rel: $($(PREFIX_TW)_TEST_FLEX_REL_OBJ)
	$(LD) -o $@ $^ -lgtest -lpthread

run_$(PREFIX_TW)parse_test: tw run_$(PREFIX_TW)flex_test $(BINDIR)/$(PREFIX_TW)test_parse-dbg $(BINDIR)/$(PREFIX_TW)test_parse-rel
	$(BINDIR)/$(PREFIX_TW)test_parse-dbg
	$(BINDIR)/$(PREFIX_TW)test_parse-rel

run_$(PREFIX_TW)flex_test: tw $(BINDIR)/$(PREFIX_TW)test_flex-dbg $(BINDIR)/$(PREFIX_TW)test_flex-rel
	$(BINDIR)/$(PREFIX_TW)test_flex-dbg
	$(BINDIR)/$(PREFIX_TW)test_flex-rel

tw_clean:
	rm -rf $(OBJDIR)/tw* $(OBJDIR)/TW_* $(BINDIR)/tw* ./$(PREFIX_TW)parser.?pp ./$(PREFIX_TW)scanner.?pp

.PHONY: tw tw_prof tw_clean run_$(PREFIX_TW)parse_test run_$(PREFIX_TW)flex_test

#################################################
# acceptance

OP2JSON = $(BINDIR)/op2jsn-dbg
GCP2JSON = $(BINDIR)/gcp2jsn-dbg
TW2JSON = $(BINDIR)/tw2jsn-dbg

jq:
	jq -e . ./expected/*/*.jsonl > /dev/null

acc_test: jq run_$(PREFIX_GCP)parse_test run_$(PREFIX_OP)parse_test run_$(PREFIX_TW)parse_test
	$(MAKE) -C aws acc_test
	#
	mkdir -p  actual/$(PREFIX_OP)testlines
	cat data/op_testlines.log | $(OP2JSON) actual/$(PREFIX_OP)testlines/op >./actual/$(PREFIX_OP)testlines/stdout 2>./actual/$(PREFIX_OP)testlines/stderr \
						&& diff ./expected/$(PREFIX_OP)testlines ./actual/$(PREFIX_OP)testlines
	@ echo "test_op passed"
	@ rm -rf actual/$(PREFIX_OP)*
	#
	mkdir -p  actual/$(PREFIX_GCP)testlines
	cat data/gcp_testlines.log | $(GCP2JSON) actual/$(PREFIX_GCP)testlines/gcp >./actual/$(PREFIX_GCP)testlines/stdout 2>./actual/$(PREFIX_GCP)testlines/stderr \
						&& diff ./expected/$(PREFIX_GCP)testlines ./actual/$(PREFIX_GCP)testlines
	@ echo "test_gcp passed"
	@ rm -rf actual/$(PREFIX_GCP)*
	#
	mkdir -p  actual/$(PREFIX_TW)testlines
	cat data/tw_testlines.log | $(TW2JSON) actual/$(PREFIX_TW)testlines/tw >./actual/$(PREFIX_TW)testlines/stdout 2>./actual/$(PREFIX_TW)testlines/stderr \
						&& diff ./expected/$(PREFIX_TW)testlines ./actual/$(PREFIX_TW)testlines
	@ echo "test_tw passed"
	@ rm -rf actual/$(PREFIX_TW)*
	@echo "done"

.PHONY: test jq

#################################################
# valgrind

VG_OPTIONS := --leak-check=full --error-exitcode=1

# which version to run under valginrd (uncomment one)
BUILD = dbg
#BUILD = rel

vg_op : $(BINDIR)/op2jsn-$(BUILD) $(BINDIR)/$(PREFIX_OP)test_flex-$(BUILD) $(BINDIR)/$(PREFIX_OP)test_parse-$(BUILD)
	valgrind $(VG_OPTIONS) $(BINDIR)/$(PREFIX_OP)test_flex-$(BUILD)
	valgrind $(VG_OPTIONS) $(BINDIR)/$(PREFIX_OP)test_parse-$(BUILD)
	cat data/op_testlines.log | valgrind $(VG_OPTIONS) $(BINDIR)/op2jsn-$(BUILD) actual/op

vg_gcp : $(BINDIR)/gcp2jsn-$(BUILD) $(BINDIR)/$(PREFIX_GCP)test_flex-$(BUILD) $(BINDIR)/$(PREFIX_GCP)test_parse-$(BUILD)
	valgrind $(VG_OPTIONS) $(BINDIR)/$(PREFIX_GCP)test_flex-$(BUILD)
	valgrind $(VG_OPTIONS) $(BINDIR)/$(PREFIX_GCP)test_parse-$(BUILD)
	cat data/gcp_testlines.log | valgrind $(VG_OPTIONS) $(BINDIR)/gcp2jsn-$(BUILD) actual/gcp

vg_aws :
	$(MAKE) -C aws vg

vg_tw : $(BINDIR)/tw2jsn-$(BUILD) $(BINDIR)/$(PREFIX_TW)test_flex-$(BUILD) $(BINDIR)/$(PREFIX_TW)test_parse-$(BUILD)
	valgrind $(VG_OPTIONS) $(BINDIR)/$(PREFIX_TW)test_flex-$(BUILD)
	valgrind $(VG_OPTIONS) $(BINDIR)/$(PREFIX_TW)test_parse-$(BUILD)
	cat data/tw_testlines.log | valgrind $(VG_OPTIONS) $(BINDIR)/tw2jsn-$(BUILD) actual/tw

vg_all : vg_op vg_gcp vg_aws vg_tw

.PHONY: vg_op vg_gcp vg_aws vg_tw vg_all

#-------------------------------------------------------------------------------
# fuzz testing

FUZZ_THREADS = 2
FUZZ_RUNS ?= 5000

FUZZ_OPT = -DTHREAD_NUM=$(FUZZ_THREADS) -fsanitize=fuzzer,address,signed-integer-overflow -fprofile-instr-generate -fcoverage-mapping

$(OBJDIR)/%.fuzz.o: %.cpp
	clang++ -c $< -o $@ -g $(CXXFLAGS) $(FUZZ_OPT)

clean_fuzz:
	$(MAKE) -C aws clean_fuzz

run_fuzz: run_aws_fuzz run_gcp_fuzz run_op_fuzz run_tw_fuzz

.phony: clean_fuzz run_fuzz

run_aws_fuzz:
	$(MAKE) -C aws run_fuzz

.phony: run_aws_fuzz

#-------------------------------------------------------------------------------
# fuzz testing GCP
#

fuzz: run_aws_fuzz run_gcp_fuzz run_op_fuzz run_tw_fuzz

.phony: fuzz

GCP_FUZZ_SRC = \
	$(GCP2JSN_CMN_SRC) \
	$(PREFIX_GCP)fuzz.cpp

GCP_FUZZ_OBJ = \
	$(addprefix $(OBJDIR)/,$(subst .cpp,.fuzz.o,$(GCP_FUZZ_SRC)))

$(BINDIR)/gcp-fuzz: gcp $(GCP_FUZZ_OBJ)
	clang++ -g -o $@ $(GCP_FUZZ_OBJ) $(FUZZ_OPT) $(COMMON_DBG_OBJ) $(LDFLAGS)

#GCP_FUZZ_DICT ?= -dict=fuzz/gcp-dict

run_gcp_fuzz: $(BINDIR)/gcp-fuzz
	 mkdir -p fuzz/gcp-corpus fuzz/gcp-seeds
	 cp data/gcp_testlines.log fuzz/gcp-seeds/
	 $^ $(GCP_FUZZ_DICT) -runs=$(FUZZ_RUNS) fuzz/gcp-corpus fuzz/gcp-seeds/

.phony: run_gcp_fuzz

#-------------------------------------------------------------------------------
# fuzz testing OP
#
OP_FUZZ_SRC = \
	$(OP2JSN_CMN_SRC) \
	$(PREFIX_OP)fuzz.cpp

OP_FUZZ_OBJ = \
	$(addprefix $(OBJDIR)/,$(subst .cpp,.fuzz.o,$(OP_FUZZ_SRC)))

$(BINDIR)/op-fuzz: op $(OP_FUZZ_OBJ)
	clang++ -g -o $@ $(OP_FUZZ_OBJ) $(FUZZ_OPT) $(COMMON_DBG_OBJ) $(LDFLAGS)

#OP_FUZZ_DICT ?= -dict=fuzz/op-dict

run_op_fuzz: $(BINDIR)/op-fuzz
	 mkdir -p fuzz/op-corpus fuzz/op-seeds
	 cp data/op_testlines.log fuzz/op-seeds/
	 $^ $(OP_FUZZ_DICT) -runs=$(FUZZ_RUNS) fuzz/op-corpus fuzz/op-seeds/

.phony: run_op_fuzz

#-------------------------------------------------------------------------------
# fuzz testing TW
#
TW_FUZZ_SRC = \
	$(TW2JSN_CMN_SRC) \
	$(PREFIX_TW)fuzz.cpp

TW_FUZZ_OBJ = \
	$(addprefix $(OBJDIR)/,$(subst .cpp,.fuzz.o,$(TW_FUZZ_SRC)))

$(BINDIR)/tw-fuzz: tw $(TW_FUZZ_OBJ)
	clang++ -g -o $@ $(TW_FUZZ_OBJ) $(FUZZ_OPT) $(COMMON_DBG_OBJ) $(LDFLAGS)

#TW_FUZZ_DICT ?= -dict=fuzz/op-dict

run_tw_fuzz: $(BINDIR)/tw-fuzz
	 mkdir -p fuzz/tw-corpus fuzz/tw-seeds
	 cp data/tw_testlines.log fuzz/tw-seeds/
	 $^ $(TW_FUZZ_DICT) -runs=$(FUZZ_RUNS) fuzz/tw-corpus fuzz/tw-seeds/

.phony: run_tw_fuzz
