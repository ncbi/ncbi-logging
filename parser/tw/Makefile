include ../Makefile.set

PREFIX_TW := tw_

all: cmn tw
	@echo "done"

cmn:
	$(MAKE) -C ../common

test: acc_test

clean: tw_clean

mkdir:
	@ mkdir -p $(BINDIR) $(OBJDIR)

.phony: all test clean mkdir

#################################################
# stand alone TW parser

TW2JSN_CMN_SRC = \
	$(PREFIX_TW)parser.cpp \
	$(PREFIX_TW)scanner.cpp \
	TW_Interface.cpp

.precious: $(PREFIX_TW)scanner.cpp $(PREFIX_TW)scanner.hpp $(PREFIX_TW)parser.cpp $(PREFIX_TW)parser.hpp

TW2JSN_SRC = $(TW2JSN_CMN_SRC) tw2jsn.cpp

TW2JSN_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$(TW2JSN_SRC)))
TW2JSN_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$(TW2JSN_SRC)))
TW2JSN_PROF_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.prof.o,$(TW2JSN_SRC)))

$(OBJDIR)/$(PREFIX_TW)parser.dbg.o:  	$(OBJDIR)/$(PREFIX_TW)scanner.dbg.o
$(OBJDIR)/$(PREFIX_TW)parser.rel.o:  	$(OBJDIR)/$(PREFIX_TW)scanner.rel.o
$(OBJDIR)/$(PREFIX_TW)parser.prof.o:  	$(OBJDIR)/$(PREFIX_TW)scanner.prof.o

$(OBJDIR)/TW_Interface.dbg.o $(OBJDIR)/TW_Interface.rel.o $(OBJDIR)/TW_Interface.prof.o : $(PREFIX_TW)parser.hpp
$(PREFIX_TW)parser.hpp : $(PREFIX_TW)parser.cpp

$(BINDIR)/tw2jsn-dbg: $(TW2JSN_DBG_OBJ) $(DBG_TOOL_OBJ)
	$(LD) -o $@ $^ $(COMMON_DBG_OBJ) $(LDFLAGS)

$(BINDIR)/tw2jsn-rel: $(TW2JSN_REL_OBJ) $(REL_TOOL_OBJ)
	$(LD) -o $@ $^ $(COMMON_REL_OBJ) $(LDFLAGS)

$(BINDIR)/tw2jsn-prof: $(TW2JSN_PROF_OBJ) $(PROF_TOOL_OBJ)
	$(LD) -o $@ $^ $(COMMON_PROF_OBJ) $(GPROFFLAGS) $(LDFLAGS)

tw: mkdir $(BINDIR)/tw2jsn-dbg $(BINDIR)/tw2jsn-rel

prof: mkdir $(BINDIR)/tw2jsn-prof
	#TODO: revive

$(PREFIX_TW)_TEST_PARSE_SRC = $(TW2JSN_CMN_SRC) $(PREFIX_TW)test_parse.cpp
$(PREFIX_TW)_TEST_FLEX_SRC = $(PREFIX_TW)test_flex.cpp $(PREFIX_TW)scanner.cpp

$(PREFIX_TW)_TEST_PARSE_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$($(PREFIX_TW)_TEST_PARSE_SRC)))
$(PREFIX_TW)_TEST_PARSE_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$($(PREFIX_TW)_TEST_PARSE_SRC)))

$(PREFIX_TW)_TEST_FLEX_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$($(PREFIX_TW)_TEST_FLEX_SRC)))
$(PREFIX_TW)_TEST_FLEX_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$($(PREFIX_TW)_TEST_FLEX_SRC)))

$(BINDIR)/$(PREFIX_TW)test_parse-dbg: $($(PREFIX_TW)_TEST_PARSE_DBG_OBJ)
	$(LD) -o $@ $^  $(COMMON_DBG_OBJ) -lgtest $(LDFLAGS)

$(BINDIR)/$(PREFIX_TW)test_parse-rel: $($(PREFIX_TW)_TEST_PARSE_REL_OBJ)
	$(LD) -o $@ $^  $(COMMON_REL_OBJ) -lgtest $(LDFLAGS)

$(BINDIR)/$(PREFIX_TW)test_flex-dbg: $($(PREFIX_TW)_TEST_FLEX_DBG_OBJ)
	$(LD) -o $@ $^ -lgtest -lpthread

$(BINDIR)/$(PREFIX_TW)test_flex-rel: $($(PREFIX_TW)_TEST_FLEX_REL_OBJ)
	$(LD) -o $@ $^ -lgtest -lpthread

parse_test: tw flex_test $(BINDIR)/$(PREFIX_TW)test_parse-dbg $(BINDIR)/$(PREFIX_TW)test_parse-rel
	$(BINDIR)/$(PREFIX_TW)test_parse-dbg
	$(BINDIR)/$(PREFIX_TW)test_parse-rel

flex_test: tw $(BINDIR)/$(PREFIX_TW)test_flex-dbg $(BINDIR)/$(PREFIX_TW)test_flex-rel
	$(BINDIR)/$(PREFIX_TW)test_flex-dbg
	$(BINDIR)/$(PREFIX_TW)test_flex-rel

.PHONY: tw prof parse_test flex_test

ALL_OBJ = $(TW2JSN_DBG_OBJ) $(TW2JSN_REL_OBJ) $(TW2JSN_PROF_OBJ) \
$($(PREFIX_TW)_TEST_PARSE_DBG_OBJ) $($(PREFIX_TW)_TEST_PARSE_REL_OBJ) \
$($(PREFIX_TW)_TEST_FLEX_DBG_OBJ) $($(PREFIX_TW)_TEST_FLEX_REL_OBJ)

ALL_D = $(subst .o,.d, $(ALL_OBJ) )

-include $(ALL_D)

tw_clean:
	@ rm -rf $(ALL_OBJ) $(ALL_D) $(BINDIR)/tw*
	@ rm -rf ./$(PREFIX_TW)parser.?pp ./$(PREFIX_TW)scanner.?pp
	@ rm -rf *.output

#################################################
# acceptance

TW2JSON = $(BINDIR)/tw2jsn-dbg

acc_test: all parse_test
	mkdir -p  actual/$(PREFIX_TW)testlines
	cat ../data/tw_testlines.log | $(TW2JSON) actual/$(PREFIX_TW)testlines/tw >./actual/$(PREFIX_TW)testlines/stdout 2>./actual/$(PREFIX_TW)testlines/stderr \
						&& diff ../expected/$(PREFIX_TW)testlines ./actual/$(PREFIX_TW)testlines
	@ echo "test_tw passed"
	@ rm -rf actual/$(PREFIX_TW)*
	@ echo "done"

.PHONY: acc_test

#################################################
# valgrind

vg : $(BINDIR)/tw2jsn-$(BUILD) $(BINDIR)/$(PREFIX_TW)test_flex-$(BUILD) $(BINDIR)/$(PREFIX_TW)test_parse-$(BUILD)
	valgrind $(VG_OPTIONS) $(BINDIR)/$(PREFIX_TW)test_flex-$(BUILD)
	valgrind $(VG_OPTIONS) $(BINDIR)/$(PREFIX_TW)test_parse-$(BUILD)
	cat data/tw_testlines.log | valgrind $(VG_OPTIONS) $(BINDIR)/tw2jsn-$(BUILD) actual/tw

.PHONY: vg

#-------------------------------------------------------------------------------
# fuzz testing

clean_fuzz:
	rm -rf fuzz/

TW_FUZZ_SRC = \
	$(TW2JSN_CMN_SRC) \
	$(PREFIX_TW)fuzz.cpp

TW_FUZZ_OBJ = \
	$(addprefix $(OBJDIR)/,$(subst .cpp,.fuzz.o,$(TW_FUZZ_SRC)))

$(BINDIR)/tw-fuzz: tw $(TW_FUZZ_OBJ)
	clang++ -g -o $@ $(TW_FUZZ_OBJ) $(FUZZ_OPT) $(COMMON_DBG_OBJ) $(LDFLAGS)

#TW_FUZZ_DICT ?= -dict=fuzz/tw-dict

fuzz: $(BINDIR)/tw-fuzz
	 mkdir -p fuzz/tw-corpus fuzz/tw-seeds
	 cp ../data/tw_testlines.log fuzz/tw-seeds/
	 $^ $(TW_FUZZ_DICT) -runs=$(FUZZ_RUNS) fuzz/tw-corpus fuzz/tw-seeds

.phony: clean_fuzz fuzz
