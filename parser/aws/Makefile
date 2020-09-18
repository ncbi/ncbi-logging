include ../Makefile.set

PREFIX := aws
CPREFIX := AWS

all: cmn $(PREFIX)
	@echo "done"

cmn:
	$(MAKE) -C ../common

test: acc_test

clean: $(PREFIX)_clean

mkdir:
	@ mkdir -p $(BINDIR) $(OBJDIR)

.phony: all test clean mkdir

#################################################
# stand alone AWS parser

$(CPREFIX)2JSN_CMN_SRC = \
	$(PREFIX)_parser.cpp \
	$(PREFIX)_scanner.cpp \
	$(CPREFIX)_Interface.cpp

.precious: $(PREFIX)_scanner.cpp $(PREFIX)_scanner.hpp $(PREFIX)_parser.cpp $(PREFIX)_parser.hpp

$(CPREFIX)2JSN_SRC = $($(CPREFIX)2JSN_CMN_SRC) $(PREFIX)2jsn.cpp
JSN2$(CPREFIX)_SRC = $($(CPREFIX)2JSN_CMN_SRC) jsn2$(PREFIX).cpp

$(CPREFIX)2JSN_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$($(CPREFIX)2JSN_SRC)))
$(CPREFIX)2JSN_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$($(CPREFIX)2JSN_SRC)))
$(CPREFIX)2JSN_PROF_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.prof.o,$($(CPREFIX)2JSN_SRC)))

JSN2$(CPREFIX)_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$(JSN2$(CPREFIX)_SRC)))
JSN2$(CPREFIX)_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$(JSN2$(CPREFIX)_SRC)))
JSN2$(CPREFIX)_PROF_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.prof.o,$(JSN2$(CPREFIX)_SRC)))

$(OBJDIR)/$(PREFIX)_parser.dbg.o:  	$(OBJDIR)/$(PREFIX)_scanner.dbg.o
$(OBJDIR)/$(PREFIX)_parser.rel.o:  	$(OBJDIR)/$(PREFIX)_scanner.rel.o
$(OBJDIR)/$(PREFIX)_parser.prof.o:  $(OBJDIR)/$(PREFIX)_scanner.prof.o

$(OBJDIR)/$(CPREFIX)_Interface.dbg.o $(OBJDIR)/$(CPREFIX)_Interface.rel.o $(OBJDIR)/$(CPREFIX)_Interface.prof.o : $(PREFIX)_parser.hpp
$(PREFIX)_parser.hpp : $(PREFIX)_parser.cpp

$(BINDIR)/$(PREFIX)2jsn-dbg: $($(CPREFIX)2JSN_DBG_OBJ) $(DBG_TOOL_OBJ)
	$(LD) -o $@ $^ $(COMMON_DBG_OBJ) $(LDFLAGS)

$(BINDIR)/$(PREFIX)2jsn-rel: $($(CPREFIX)2JSN_REL_OBJ) $(REL_TOOL_OBJ)
	$(LD) -o $@ $^ $(COMMON_REL_OBJ) $(LDFLAGS)

$(BINDIR)/$(PREFIX)2jsn-prof: $($(CPREFIX)2JSN_PROF_OBJ) $(PROF_TOOL_OBJ)
	$(LD) -o $@ $^ $(COMMON_PROF_OBJ) $(GPROFFLAGS) $(LDFLAGS)

$(BINDIR)/jsn2$(PREFIX)-dbg: $(JSN2$(CPREFIX)_DBG_OBJ) $(DBG_TOOL_OBJ)
	$(LD) -o $@ $^ $(COMMON_DBG_OBJ) $(LDFLAGS)

$(BINDIR)/jsn2$(PREFIX)-rel: $(JSN2$(CPREFIX)_REL_OBJ) $(REL_TOOL_OBJ)
	$(LD) -o $@ $^ $(COMMON_REL_OBJ) $(LDFLAGS)

$(BINDIR)/jsn2$(PREFIX)-prof: $(JSN2$(CPREFIX)_PROF_OBJ) $(PROF_TOOL_OBJ)
	$(LD) -o $@ $^ $(COMMON_PROF_OBJ) $(GPROFFLAGS) $(LDFLAGS)

$(PREFIX): mkdir $(BINDIR)/$(PREFIX)2jsn-dbg $(BINDIR)/$(PREFIX)2jsn-rel $(BINDIR)/jsn2$(PREFIX)-dbg $(BINDIR)/jsn2$(PREFIX)-rel

prof: mkdir $(BINDIR)/$(PREFIX)2jsn-prof mkdir $(BINDIR)/jsn2$(PREFIX)-prof
	#TODO: revive

$(PREFIX)_TEST_PARSE_SRC = $($(CPREFIX)2JSN_CMN_SRC) $(PREFIX)_test_parse.cpp
$(PREFIX)_TEST_FLEX_SRC = $(PREFIX)_test_flex.cpp $(PREFIX)_scanner.cpp
$(PREFIX)_TEST_REVERSE_SRC = $(PREFIX)_test_reverse.cpp $(PREFIX)_parser.cpp $(PREFIX)_scanner.cpp

$(PREFIX)_TEST_PARSE_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$($(PREFIX)_TEST_PARSE_SRC)))
$(PREFIX)_TEST_PARSE_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$($(PREFIX)_TEST_PARSE_SRC)))

$(PREFIX)_TEST_FLEX_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$($(PREFIX)_TEST_FLEX_SRC)))
$(PREFIX)_TEST_FLEX_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$($(PREFIX)_TEST_FLEX_SRC)))

$(PREFIX)_TEST_REVERSE_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$($(PREFIX)_TEST_REVERSE_SRC)))
$(PREFIX)_TEST_REVERSE_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$($(PREFIX)_TEST_REVERSE_SRC)))

$(BINDIR)/$(PREFIX)_test_parse-dbg: $($(PREFIX)_TEST_PARSE_DBG_OBJ)
	$(LD) -o $@ $^  $(COMMON_DBG_OBJ) -lgtest $(LDFLAGS)

$(BINDIR)/$(PREFIX)_test_parse-rel: $($(PREFIX)_TEST_PARSE_REL_OBJ)
	$(LD) -o $@ $^  $(COMMON_REL_OBJ) -lgtest $(LDFLAGS)

$(BINDIR)/$(PREFIX)_test_flex-dbg: $($(PREFIX)_TEST_FLEX_DBG_OBJ)
	$(LD) -o $@ $^ -lgtest -lpthread

$(BINDIR)/$(PREFIX)_test_flex-rel: $($(PREFIX)_TEST_FLEX_REL_OBJ)
	$(LD) -o $@ $^ -lgtest -lpthread

#$(BINDIR)/$(PREFIX)_test_reverse-dbg: $(CPREFIX)_Interface.cpp
$(BINDIR)/$(PREFIX)_test_reverse-dbg: $($(PREFIX)_TEST_REVERSE_DBG_OBJ)
	$(LD) -o $@ $^ $(COMMON_DBG_OBJ) -lgtest $(LDFLAGS)

#$(BINDIR)/$(PREFIX)_test_reverse-rel: $(CPREFIX)_Interface.cpp
$(BINDIR)/$(PREFIX)_test_reverse-rel: $($(PREFIX)_TEST_REVERSE_REL_OBJ)
	$(LD) -o $@ $^ $(COMMON_REL_OBJ) -lgtest $(LDFLAGS)

unittest_parse: $(PREFIX) flex_test $(BINDIR)/$(PREFIX)_test_parse-dbg $(BINDIR)/$(PREFIX)_test_parse-rel
	$(BINDIR)/$(PREFIX)_test_parse-dbg
	$(BINDIR)/$(PREFIX)_test_parse-rel

flex_test: $(PREFIX) $(BINDIR)/$(PREFIX)_test_flex-dbg $(BINDIR)/$(PREFIX)_test_flex-rel
	$(BINDIR)/$(PREFIX)_test_flex-dbg
	$(BINDIR)/$(PREFIX)_test_flex-rel

unittest_reverse: $(PREFIX) $(BINDIR)/$(PREFIX)_test_reverse-dbg $(BINDIR)/$(PREFIX)_test_reverse-rel
	$(BINDIR)/$(PREFIX)_test_reverse-dbg
	$(BINDIR)/$(PREFIX)_test_reverse-rel

.PHONY: $(PREFIX) prof unittest_parse flex_test unittest_reverse

ALL_OBJ = $($(CPREFIX)2JSN_DBG_OBJ) $($(CPREFIX)2JSN_REL_OBJ) $($(CPREFIX)2JSN_PROF_OBJ) \
$(JSN2$(CPREFIX)_DBG_OBJ) $(JSN2$(CPREFIX)_REL_OBJ) $(JSN2$(CPREFIX)_PROF_OBJ) \
$($(PREFIX)_TEST_PARSE_DBG_OBJ) $($(PREFIX)_TEST_PARSE_REL_OBJ) \
$($(PREFIX)_TEST_FLEX_DBG_OBJ) $($(PREFIX)_TEST_FLEX_REL_OBJ) \
$($(PREFIX)_TEST_REVERSE_DBG_OBJ) $($(PREFIX)_TEST_REVERSE_REL_OBJ)

ALL_D = $(subst .o,.d, $(ALL_OBJ) )

-include $(ALL_D)

$(PREFIX)_clean:
	@ rm -rf $(ALL_OBJ) $(ALL_D) $(BINDIR)/$(PREFIX)* $(BINDIR)/jsn2$(PREFIX)*
	@ rm -rf ./$(PREFIX)_parser.?pp ./$(PREFIX)_scanner.?pp
	@ rm -rf *.output

#################################################
# acceptance

$(CPREFIX)2JSON = $(BINDIR)/$(PREFIX)2jsn-dbg
JSON2$(CPREFIX) = $(BINDIR)/jsn2$(PREFIX)-dbg

acc_test: all unittest_parse unittest_reverse acctest_parse acctest_reverse
	@ rm -rf actual/$(PREFIX)_*
	@ echo "$(PREFIX) tests done"

acctest_parse: all
	mkdir -p actual/$(PREFIX)_testlines
	cat ../data/$(PREFIX)_testlines.log | $($(CPREFIX)2JSON) actual/$(PREFIX)_testlines/$(PREFIX) >./actual/$(PREFIX)_testlines/stdout 2>./actual/$(PREFIX)_testlines/stderr \
						&& diff ../expected/$(PREFIX)_testlines ./actual/$(PREFIX)_testlines
	@ echo "$(PREFIX) acctest_parse passed"

JSON1 = ../data/$(PREFIX)_reparse_1.json
GEN1_DIR = actual/gen1

JSON1_MT = ../data/$(PREFIX)_reparse_mt.json
GEN1_MT_DIR = actual/gen1_mt

RT_JSN = ../data/$(PREFIX)_roundtrip.json
RT_ORIG1_DIR = actual/rt_orig1
RT_JSN_DIR = actual/rt_json
RT_ORIG2_DIR = actual/rt_orig2

acctest_reverse: all
	#
	mkdir -p $(GEN1_DIR)
	cat $(JSON1) | $(JSON2$(CPREFIX)) $(GEN1_DIR)/$(PREFIX) >$(GEN1_DIR)/stdout 2>$(GEN1_DIR)/stderr \
		&& diff ../expected/$(PREFIX)_rev_testlines $(GEN1_DIR)
	#
	# multi-threaded
	mkdir -p $(GEN1_MT_DIR)
	cat $(JSON1_MT) | $(JSON2$(CPREFIX)) $(GEN1_MT_DIR)/$(PREFIX) -t 2 >$(GEN1_MT_DIR)/stdout 2>$(GEN1_MT_DIR)/stderr \
		&& diff ../expected/$(PREFIX)_reparse_mt $(GEN1_MT_DIR)
	#
	# round trip
	mkdir -p $(RT_ORIG1_DIR) $(RT_JSN_DIR) $(RT_ORIG2_DIR)
	# reparse 1
	cat $(RT_JSN) | $(JSON2$(CPREFIX)) $(RT_ORIG1_DIR)/$(PREFIX) >$(RT_ORIG1_DIR)/stdout 2>$(RT_ORIG1_DIR)/stderr
	# forward parse the original format
	cat $(RT_ORIG1_DIR)/$(PREFIX).good.$(PREFIX) | $($(CPREFIX)2JSON) $(RT_JSN_DIR)/ >$(RT_JSN_DIR)/stdout 2>$(RT_JSN_DIR)/stderr
	# reparse 2, compare the output of 2 reparses
	cat $(RT_JSN_DIR)/.good.jsonl | $(JSON2$(CPREFIX)) $(RT_ORIG2_DIR)/$(PREFIX) >$(RT_ORIG2_DIR)/stdout 2>$(RT_ORIG2_DIR)/stderr && diff $(RT_ORIG1_DIR) $(RT_ORIG2_DIR)
	#
	@ echo "$(PREFIX) acctest_reverse passed"

.PHONY: acc_test acctest_parse acctest_reverse

#################################################
# valgrind

vg : $(BINDIR)/$(PREFIX)2jsn-$(BUILD) $(BINDIR)/$(PREFIX)_test_flex-$(BUILD) $(BINDIR)/$(PREFIX)_test_parse-$(BUILD)
	valgrind $(VG_OPTIONS) $(BINDIR)/$(PREFIX)_test_flex-$(BUILD)
	valgrind $(VG_OPTIONS) $(BINDIR)/$(PREFIX)_test_parse-$(BUILD)
	cat data/$(PREFIX)_testlines.log | valgrind $(VG_OPTIONS) $(BINDIR)/$(PREFIX)2jsn-$(BUILD) actual/$(PREFIX)

.PHONY: vg

#-------------------------------------------------------------------------------
# fuzz testing

clean_fuzz:
	rm -rf fuzz/

FUZZ_SRC = \
	$($(CPREFIX)2JSN_CMN_SRC) \
	$(PREFIX)_fuzz.cpp

FUZZ_OBJ = \
	$(addprefix $(OBJDIR)/,$(subst .cpp,.fuzz.o,$(FUZZ_SRC)))

$(BINDIR)/$(PREFIX)-fuzz: $(PREFIX) $(FUZZ_OBJ)
	clang++ -g -o $@ $(FUZZ_OBJ) $(FUZZ_OPT) $(COMMON_DBG_OBJ) $(LDFLAGS)

#$(CPREFIX)_FUZZ_DICT ?= -dict=fuzz/$(PREFIX)-dict

fuzz: $(BINDIR)/$(PREFIX)-fuzz
	 mkdir -p fuzz/$(PREFIX)-corpus fuzz/$(PREFIX)-seeds
	 cp ../data/$(PREFIX)_testlines.log fuzz/$(PREFIX)-seeds/
	 $^ $($(CPREFIX)_FUZZ_DICT) -runs=$(FUZZ_RUNS) fuzz/$(PREFIX)-corpus fuzz/$(PREFIX)-seeds

.phony: clean_fuzz fuzz
