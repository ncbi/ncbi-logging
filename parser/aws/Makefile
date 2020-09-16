include ../Makefile.set

PREFIX_AWS := aws_

all: cmn aws
	@echo "done"

cmn:
	$(MAKE) -C ../common

test: acc_test

clean: aws_clean

mkdir:
	@ mkdir -p $(BINDIR) $(OBJDIR)

.phony: all test clean mkdir

#################################################
# stand alone AWS parser

AWS2JSN_CMN_SRC = \
	$(PREFIX_AWS)parser.cpp \
	$(PREFIX_AWS)scanner.cpp \
	AWS_Interface.cpp

.precious: $(PREFIX_AWS)scanner.cpp $(PREFIX_AWS)scanner.hpp $(PREFIX_AWS)parser.cpp $(PREFIX_AWS)parser.hpp

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

prof: mkdir $(BINDIR)/aws2jsn-prof
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

parse_test: aws flex_test $(BINDIR)/$(PREFIX_AWS)test_parse-dbg $(BINDIR)/$(PREFIX_AWS)test_parse-rel
	$(BINDIR)/$(PREFIX_AWS)test_parse-dbg
	$(BINDIR)/$(PREFIX_AWS)test_parse-rel

flex_test: aws $(BINDIR)/$(PREFIX_AWS)test_flex-dbg $(BINDIR)/$(PREFIX_AWS)test_flex-rel
	$(BINDIR)/$(PREFIX_AWS)test_flex-dbg
	$(BINDIR)/$(PREFIX_AWS)test_flex-rel

.PHONY: aws prof parse_test flex_test

ALL_OBJ = $(AWS2JSN_DBG_OBJ) $(AWS2JSN_REL_OBJ) $(AWS2JSN_PROF_OBJ) \
$($(PREFIX_AWS)_TEST_PARSE_DBG_OBJ) $($(PREFIX_AWS)_TEST_PARSE_REL_OBJ) \
$($(PREFIX_AWS)_TEST_FLEX_DBG_OBJ) $($(PREFIX_AWS)_TEST_FLEX_REL_OBJ)

ALL_D = $(subst .o,.d, $(ALL_OBJ) )

-include $(ALL_D)

aws_clean:
	@ rm -rf $(ALL_OBJ) $(ALL_D) $(BINDIR)/aws*
	@ rm -rf ./$(PREFIX_AWS)parser.?pp ./$(PREFIX_AWS)scanner.?pp
	@ rm -rf *.output

#################################################
# acceptance

AWS2JSON = $(BINDIR)/aws2jsn-dbg

acc_test: all parse_test
	mkdir -p  actual/$(PREFIX_AWS)testlines
	cat ../data/aws_testlines.log | $(AWS2JSON) actual/$(PREFIX_AWS)testlines/aws >./actual/$(PREFIX_AWS)testlines/stdout 2>./actual/$(PREFIX_AWS)testlines/stderr \
						&& diff ../expected/$(PREFIX_AWS)testlines ./actual/$(PREFIX_AWS)testlines
	@ echo "test_aws passed"
	@ rm -rf actual/$(PREFIX_AWS)*
	@ echo "done"

.PHONY: acc_test

#################################################
# valgrind

vg : $(BINDIR)/aws2jsn-$(BUILD) $(BINDIR)/$(PREFIX_AWS)test_flex-$(BUILD) $(BINDIR)/$(PREFIX_AWS)test_parse-$(BUILD)
	valgrind $(VG_OPTIONS) $(BINDIR)/$(PREFIX_AWS)test_flex-$(BUILD)
	valgrind $(VG_OPTIONS) $(BINDIR)/$(PREFIX_AWS)test_parse-$(BUILD)
	cat data/aws_testlines.log | valgrind $(VG_OPTIONS) $(BINDIR)/aws2jsn-$(BUILD) actual/aws

.PHONY: vg

#-------------------------------------------------------------------------------
# fuzz testing

clean_fuzz:
	rm -rf fuzz/

AWS_FUZZ_SRC = \
	$(AWS2JSN_CMN_SRC) \
	$(PREFIX_AWS)fuzz.cpp

AWS_FUZZ_OBJ = \
	$(addprefix $(OBJDIR)/,$(subst .cpp,.fuzz.o,$(AWS_FUZZ_SRC)))

$(BINDIR)/aws-fuzz: aws $(AWS_FUZZ_OBJ)
	clang++ -g -o $@ $(AWS_FUZZ_OBJ) $(FUZZ_OPT) $(COMMON_DBG_OBJ) $(LDFLAGS)

#AWS_FUZZ_DICT ?= -dict=fuzz/aws-dict

fuzz: $(BINDIR)/aws-fuzz
	 mkdir -p fuzz/aws-corpus fuzz/aws-seeds
	 cp ../data/aws_testlines.log fuzz/aws-seeds/
	 $^ $(AWS_FUZZ_DICT) -runs=$(FUZZ_RUNS) fuzz/aws-corpus fuzz/aws-seeds

.phony: clean_fuzz fuzz
