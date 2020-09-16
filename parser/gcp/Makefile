include ../Makefile.set

PREFIX_GCP := gcp_

all: cmn gcp
	@echo "done"

cmn:
	$(MAKE) -C ../common

test: acc_test

clean: gcp_clean

mkdir:
	@ mkdir -p $(BINDIR) $(OBJDIR)

.phony: all test clean mkdir

#################################################
# stand alone GCP parser

GCP2JSN_CMN_SRC = \
	$(PREFIX_GCP)parser.cpp \
	$(PREFIX_GCP)scanner.cpp \
	GCP_Interface.cpp

.precious: $(PREFIX_GCP)scanner.cpp $(PREFIX_GCP)scanner.hpp $(PREFIX_GCP)parser.cpp $(PREFIX_GCP)parser.hpp

GCP2JSN_SRC = $(GCP2JSN_CMN_SRC) gcp2jsn.cpp

GCP2JSN_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$(GCP2JSN_SRC)))
GCP2JSN_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$(GCP2JSN_SRC)))
GCP2JSN_PROF_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.prof.o,$(GCP2JSN_SRC)))

$(OBJDIR)/$(PREFIX_GCP)parser.dbg.o:  	$(OBJDIR)/$(PREFIX_GCP)scanner.dbg.o
$(OBJDIR)/$(PREFIX_GCP)parser.rel.o:  	$(OBJDIR)/$(PREFIX_GCP)scanner.rel.o
$(OBJDIR)/$(PREFIX_GCP)parser.prof.o:  	$(OBJDIR)/$(PREFIX_GCP)scanner.prof.o

$(OBJDIR)/GCP_Interface.dbg.o $(OBJDIR)/GCP_Interface.rel.o $(OBJDIR)/GCP_Interface.prof.o : $(PREFIX_GCP)parser.hpp
$(PREFIX_GCP)parser.hpp : $(PREFIX_GCP)parser.cpp

$(BINDIR)/gcp2jsn-dbg: $(GCP2JSN_DBG_OBJ) $(DBG_TOOL_OBJ)
	$(LD) -o $@ $^ $(COMMON_DBG_OBJ) $(LDFLAGS)

$(BINDIR)/gcp2jsn-rel: $(GCP2JSN_REL_OBJ) $(REL_TOOL_OBJ)
	$(LD) -o $@ $^ $(COMMON_REL_OBJ) $(LDFLAGS)

$(BINDIR)/gcp2jsn-prof: $(GCP2JSN_PROF_OBJ) $(PROF_TOOL_OBJ)
	$(LD) -o $@ $^ $(COMMON_PROF_OBJ) $(GPROFFLAGS) $(LDFLAGS)

gcp: mkdir $(BINDIR)/gcp2jsn-dbg $(BINDIR)/gcp2jsn-rel

prof: mkdir $(BINDIR)/gcp2jsn-prof
	#TODO: revive

$(PREFIX_GCP)_TEST_PARSE_SRC = $(GCP2JSN_CMN_SRC) $(PREFIX_GCP)test_parse.cpp
$(PREFIX_GCP)_TEST_FLEX_SRC = $(PREFIX_GCP)test_flex.cpp $(PREFIX_GCP)scanner.cpp

$(PREFIX_GCP)_TEST_PARSE_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$($(PREFIX_GCP)_TEST_PARSE_SRC)))
$(PREFIX_GCP)_TEST_PARSE_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$($(PREFIX_GCP)_TEST_PARSE_SRC)))

$(PREFIX_GCP)_TEST_FLEX_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$($(PREFIX_GCP)_TEST_FLEX_SRC)))
$(PREFIX_GCP)_TEST_FLEX_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$($(PREFIX_GCP)_TEST_FLEX_SRC)))

$(BINDIR)/$(PREFIX_GCP)test_parse-dbg: $($(PREFIX_GCP)_TEST_PARSE_DBG_OBJ)
	$(LD) -o $@ $^  $(COMMON_DBG_OBJ) -lgtest $(LDFLAGS)

$(BINDIR)/$(PREFIX_GCP)test_parse-rel: $($(PREFIX_GCP)_TEST_PARSE_REL_OBJ)
	$(LD) -o $@ $^  $(COMMON_REL_OBJ) -lgtest $(LDFLAGS)

$(BINDIR)/$(PREFIX_GCP)test_flex-dbg: $($(PREFIX_GCP)_TEST_FLEX_DBG_OBJ)
	$(LD) -o $@ $^ -lgtest -lpthread

$(BINDIR)/$(PREFIX_GCP)test_flex-rel: $($(PREFIX_GCP)_TEST_FLEX_REL_OBJ)
	$(LD) -o $@ $^ -lgtest -lpthread

parse_test: gcp flex_test $(BINDIR)/$(PREFIX_GCP)test_parse-dbg $(BINDIR)/$(PREFIX_GCP)test_parse-rel
	$(BINDIR)/$(PREFIX_GCP)test_parse-dbg
	$(BINDIR)/$(PREFIX_GCP)test_parse-rel

flex_test: gcp $(BINDIR)/$(PREFIX_GCP)test_flex-dbg $(BINDIR)/$(PREFIX_GCP)test_flex-rel
	$(BINDIR)/$(PREFIX_GCP)test_flex-dbg
	$(BINDIR)/$(PREFIX_GCP)test_flex-rel

.PHONY: gcp prof parse_test flex_test

ALL_OBJ = $(GCP2JSN_DBG_OBJ) $(GCP2JSN_REL_OBJ) $(GCP2JSN_PROF_OBJ) \
$($(PREFIX_GCP)_TEST_PARSE_DBG_OBJ) $($(PREFIX_GCP)_TEST_PARSE_REL_OBJ) \
$($(PREFIX_GCP)_TEST_FLEX_DBG_OBJ) $($(PREFIX_GCP)_TEST_FLEX_REL_OBJ)

ALL_D = $(subst .o,.d, $(ALL_OBJ) )

-include $(ALL_D)

gcp_clean:
	@ rm -rf $(ALL_OBJ) $(ALL_D) $(BINDIR)/gcp*
	@ rm -rf ./$(PREFIX_GCP)parser.?pp ./$(PREFIX_GCP)scanner.?pp
	@ rm -rf *.output

#################################################
# acceptance

GCP2JSON = $(BINDIR)/gcp2jsn-dbg

acc_test: all parse_test
	mkdir -p  actual/$(PREFIX_GCP)testlines
	cat ../data/gcp_testlines.log | $(GCP2JSON) actual/$(PREFIX_GCP)testlines/gcp >./actual/$(PREFIX_GCP)testlines/stdout 2>./actual/$(PREFIX_GCP)testlines/stderr \
						&& diff ../expected/$(PREFIX_GCP)testlines ./actual/$(PREFIX_GCP)testlines
	@ echo "test_gcp passed"
	@ rm -rf actual/$(PREFIX_GCP)*
	@ echo "done"

.PHONY: acc_test

#################################################
# valgrind

vg : $(BINDIR)/gcp2jsn-$(BUILD) $(BINDIR)/$(PREFIX_GCP)test_flex-$(BUILD) $(BINDIR)/$(PREFIX_GCP)test_parse-$(BUILD)
	valgrind $(VG_OPTIONS) $(BINDIR)/$(PREFIX_GCP)test_flex-$(BUILD)
	valgrind $(VG_OPTIONS) $(BINDIR)/$(PREFIX_GCP)test_parse-$(BUILD)
	cat data/gcp_testlines.log | valgrind $(VG_OPTIONS) $(BINDIR)/gcp2jsn-$(BUILD) actual/gcp

.PHONY: vg

#-------------------------------------------------------------------------------
# fuzz testing

clean_fuzz:
	rm -rf fuzz/

GCP_FUZZ_SRC = \
	$(GCP2JSN_CMN_SRC) \
	$(PREFIX_GCP)fuzz.cpp

GCP_FUZZ_OBJ = \
	$(addprefix $(OBJDIR)/,$(subst .cpp,.fuzz.o,$(GCP_FUZZ_SRC)))

$(BINDIR)/gcp-fuzz: gcp $(GCP_FUZZ_OBJ)
	clang++ -g -o $@ $(GCP_FUZZ_OBJ) $(FUZZ_OPT) $(COMMON_DBG_OBJ) $(LDFLAGS)

#GCP_FUZZ_DICT ?= -dict=fuzz/gcp-dict

fuzz: $(BINDIR)/gcp-fuzz
	 mkdir -p fuzz/gcp-corpus fuzz/gcp-seeds
	 cp ../data/gcp_testlines.log fuzz/gcp-seeds/
	 $^ $(GCP_FUZZ_DICT) -runs=$(FUZZ_RUNS) fuzz/gcp-corpus fuzz/gcp-seeds

.phony: clean_fuzz fuzz

