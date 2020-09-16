include ../Makefile.set

PREFIX_OP := op_

all: cmn op
	@echo "done"

cmn:
	$(MAKE) -C ../common

test: acc_test

clean: op_clean

mkdir:
	@ mkdir -p $(BINDIR) $(OBJDIR)

.phony: all test clean mkdir

#################################################
# stand alone OP parser

OP2JSN_CMN_SRC = \
	$(PREFIX_OP)parser.cpp \
	$(PREFIX_OP)scanner.cpp \
	OP_Interface.cpp

.precious: $(PREFIX_OP)scanner.cpp $(PREFIX_OP)scanner.hpp $(PREFIX_OP)parser.cpp $(PREFIX_OP)parser.hpp

OP2JSN_SRC = $(OP2JSN_CMN_SRC) op2jsn.cpp

OP2JSN_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$(OP2JSN_SRC)))
OP2JSN_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$(OP2JSN_SRC)))
OP2JSN_PROF_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.prof.o,$(OP2JSN_SRC)))

$(OBJDIR)/$(PREFIX_OP)parser.dbg.o:  	$(OBJDIR)/$(PREFIX_OP)scanner.dbg.o
$(OBJDIR)/$(PREFIX_OP)parser.rel.o:  	$(OBJDIR)/$(PREFIX_OP)scanner.rel.o
$(OBJDIR)/$(PREFIX_OP)parser.prof.o:  	$(OBJDIR)/$(PREFIX_OP)scanner.prof.o

$(OBJDIR)/OP_Interface.dbg.o $(OBJDIR)/OP_Interface.rel.o $(OBJDIR)/OP_Interface.prof.o : $(PREFIX_OP)parser.hpp
$(PREFIX_OP)parser.hpp : $(PREFIX_OP)parser.cpp

$(BINDIR)/op2jsn-dbg: $(OP2JSN_DBG_OBJ) $(DBG_TOOL_OBJ)
	$(LD) -o $@ $^ $(COMMON_DBG_OBJ) $(LDFLAGS)

$(BINDIR)/op2jsn-rel: $(OP2JSN_REL_OBJ) $(REL_TOOL_OBJ)
	$(LD) -o $@ $^ $(COMMON_REL_OBJ) $(LDFLAGS)

$(BINDIR)/op2jsn-prof: $(OP2JSN_PROF_OBJ) $(PROF_TOOL_OBJ)
	$(LD) -o $@ $^ $(COMMON_PROF_OBJ) $(GPROFFLAGS) $(LDFLAGS)

op: mkdir $(BINDIR)/op2jsn-dbg $(BINDIR)/op2jsn-rel

prof: mkdir $(BINDIR)/op2jsn-prof
	#TODO: revive

$(PREFIX_OP)_TEST_PARSE_SRC = $(OP2JSN_CMN_SRC) $(PREFIX_OP)test_parse.cpp
$(PREFIX_OP)_TEST_FLEX_SRC = $(PREFIX_OP)test_flex.cpp $(PREFIX_OP)scanner.cpp

$(PREFIX_OP)_TEST_PARSE_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$($(PREFIX_OP)_TEST_PARSE_SRC)))
$(PREFIX_OP)_TEST_PARSE_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$($(PREFIX_OP)_TEST_PARSE_SRC)))

$(PREFIX_OP)_TEST_FLEX_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$($(PREFIX_OP)_TEST_FLEX_SRC)))
$(PREFIX_OP)_TEST_FLEX_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$($(PREFIX_OP)_TEST_FLEX_SRC)))

$(BINDIR)/$(PREFIX_OP)test_parse-dbg: $($(PREFIX_OP)_TEST_PARSE_DBG_OBJ)
	$(LD) -o $@ $^  $(COMMON_DBG_OBJ) -lgtest $(LDFLAGS)

$(BINDIR)/$(PREFIX_OP)test_parse-rel: $($(PREFIX_OP)_TEST_PARSE_REL_OBJ)
	$(LD) -o $@ $^  $(COMMON_REL_OBJ) -lgtest $(LDFLAGS)

$(BINDIR)/$(PREFIX_OP)test_flex-dbg: $($(PREFIX_OP)_TEST_FLEX_DBG_OBJ)
	$(LD) -o $@ $^ -lgtest -lpthread

$(BINDIR)/$(PREFIX_OP)test_flex-rel: $($(PREFIX_OP)_TEST_FLEX_REL_OBJ)
	$(LD) -o $@ $^ -lgtest -lpthread

parse_test: op flex_test $(BINDIR)/$(PREFIX_OP)test_parse-dbg $(BINDIR)/$(PREFIX_OP)test_parse-rel
	$(BINDIR)/$(PREFIX_OP)test_parse-dbg
	$(BINDIR)/$(PREFIX_OP)test_parse-rel

flex_test: op $(BINDIR)/$(PREFIX_OP)test_flex-dbg $(BINDIR)/$(PREFIX_OP)test_flex-rel
	$(BINDIR)/$(PREFIX_OP)test_flex-dbg
	$(BINDIR)/$(PREFIX_OP)test_flex-rel

.PHONY: op prof parse_test flex_test

ALL_OBJ = $(OP2JSN_DBG_OBJ) $(OP2JSN_REL_OBJ) $(OP2JSN_PROF_OBJ) \
$($(PREFIX_OP)_TEST_PARSE_DBG_OBJ) $($(PREFIX_OP)_TEST_PARSE_REL_OBJ) \
$($(PREFIX_OP)_TEST_FLEX_DBG_OBJ) $($(PREFIX_OP)_TEST_FLEX_REL_OBJ)

ALL_D = $(subst .o,.d, $(ALL_OBJ) )

-include $(ALL_D)

op_clean:
	@ rm -rf $(ALL_OBJ) $(ALL_D) $(BINDIR)/op*
	@ rm -rf ./$(PREFIX_OP)parser.?pp ./$(PREFIX_OP)scanner.?pp
	@ rm -rf *.output

#################################################
# acceptance

OP2JSON = $(BINDIR)/op2jsn-dbg

acc_test: all parse_test
	mkdir -p  actual/$(PREFIX_OP)testlines
	cat ../data/op_testlines.log | $(OP2JSON) actual/$(PREFIX_OP)testlines/op >./actual/$(PREFIX_OP)testlines/stdout 2>./actual/$(PREFIX_OP)testlines/stderr \
						&& diff ../expected/$(PREFIX_OP)testlines ./actual/$(PREFIX_OP)testlines
	@ echo "test_op passed"
	@ rm -rf actual/$(PREFIX_OP)*
	@ echo "done"

.PHONY: acc_test

#################################################
# valgrind

vg : $(BINDIR)/op2jsn-$(BUILD) $(BINDIR)/$(PREFIX_OP)test_flex-$(BUILD) $(BINDIR)/$(PREFIX_OP)test_parse-$(BUILD)
	valgrind $(VG_OPTIONS) $(BINDIR)/$(PREFIX_OP)test_flex-$(BUILD)
	valgrind $(VG_OPTIONS) $(BINDIR)/$(PREFIX_OP)test_parse-$(BUILD)
	cat data/op_testlines.log | valgrind $(VG_OPTIONS) $(BINDIR)/op2jsn-$(BUILD) actual/op

.PHONY: vg

#-------------------------------------------------------------------------------
# fuzz testing

clean_fuzz:
	rm -rf fuzz/

OP_FUZZ_SRC = \
	$(OP2JSN_CMN_SRC) \
	$(PREFIX_OP)fuzz.cpp

OP_FUZZ_OBJ = \
	$(addprefix $(OBJDIR)/,$(subst .cpp,.fuzz.o,$(OP_FUZZ_SRC)))

$(BINDIR)/op-fuzz: op $(OP_FUZZ_OBJ)
	clang++ -g -o $@ $(OP_FUZZ_OBJ) $(FUZZ_OPT) $(COMMON_DBG_OBJ) $(LDFLAGS)

#OP_FUZZ_DICT ?= -dict=fuzz/op-dict

fuzz: $(BINDIR)/op-fuzz
	 mkdir -p fuzz/op-corpus fuzz/op-seeds
	 cp ../data/op_testlines.log fuzz/op-seeds/
	 $^ $(OP_FUZZ_DICT) -runs=$(FUZZ_RUNS) fuzz/op-corpus fuzz/op-seeds

.phony: clean_fuzz fuzz

