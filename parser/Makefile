BINDIR = bin
LIBDIR = lib
OBJDIR = obj

CC := gcc -c
CPP := g++ -c
LD := g++
CXXFLAGS += -I . -MMD
GPROFFLAGS = -pg -O3
FLEXFLAGS = -8 --align

PREFIX_OP := op_
PREFIX_AWS := aws_
PREFIX_GCP := gcp_
PREFIX_TW := tw_

all: aws op gcp tw
	@echo "done"

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
	rm -f $(PREFIX_TW)parser.hpp $(PREFIX_TW)parser.cpp
	rm -f $(PREFIX_TW)scanner.hpp $(PREFIX_TW)scanner.cpp
	rm -f $(PREFIX_TW)parser.output

mkdir:
	@ mkdir -p $(LIBDIR) $(BINDIR) $(OBJDIR)

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

.phony: all all_test clean mkdir

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
# utility modules

COMMON_SRC = \
	CatWriters.cpp \
	Formatters.cpp \
	ReceiverInterface.cpp \
	Queues.cpp \
	LineSplitters.cpp

#################################################
# unit tests for formatter/catwriter/etc

TEST_SUPPORT_SRC = $(COMMON_SRC) \
	test_formatter.cpp \
	test_catwriter.cpp \
	test_queues.cpp \
	test_linesplitter.cpp \
	test_support_main.cpp

TEST_SUPPORT_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$(TEST_SUPPORT_SRC)))
TEST_SUPPORT_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$(TEST_SUPPORT_SRC)))

$(BINDIR)/test_support-dbg: $(TEST_SUPPORT_DBG_OBJ)
	$(LD) -o $@ $^ -L$(JWT_TOOL)/lib -lncbi-json-dbg -lncbi-secure-dbg -lutf8proc -lmbedcrypto -lgtest -lpthread

$(BINDIR)/test_support-rel: $(TEST_SUPPORT_REL_OBJ)
	$(LD) -o $@ $^ -L$(JWT_TOOL)/lib -lncbi-json-rel -lncbi-secure-rel -lutf8proc -lmbedcrypto -lgtest -lpthread

run_test_support: mkdir $(BINDIR)/test_support-dbg $(BINDIR)/test_support-rel
	$(BINDIR)/test_support-dbg
	$(BINDIR)/test_support-rel

.PHONY: run_test_support

#################################################
# generic app head

DBG_TOOL_OBJ = $(OBJDIR)/Tool.dbg.o $(DBG_CMD_OBJ)
REL_TOOL_OBJ = $(OBJDIR)/Tool.rel.o $(REL_CMD_OBJ)
PROF_TOOL_OBJ = $(OBJDIR)/Tool.prof.o $(PROF_CMD_OBJ)

LDFLAGS = -lutf8proc -lmbedcrypto -lpthread

#################################################
# stand alone AWS parser

AWS2JSN_CMN_SRC = \
	$(COMMON_SRC) \
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
	$(LD) -o $@ $^ -L$(JWT_TOOL)/lib -lncbi-json-dbg -lncbi-secure-dbg $(LDFLAGS)

$(BINDIR)/aws2jsn-rel: $(AWS2JSN_REL_OBJ) $(REL_TOOL_OBJ)
	$(LD) -o $@ $^ -L$(JWT_TOOL)/lib -lncbi-json-rel -lncbi-secure-rel $(LDFLAGS)

$(BINDIR)/aws2jsn-prof: $(AWS2JSN_PROF_OBJ) $(PROF_TOOL_OBJ)
	$(LD) -o $@ $^ $(GPROFFLAGS) -L$(JWT_TOOL)/lib -lncbi-json-rel -lncbi-secure-rel $(LDFLAGS)

aws: mkdir $(BINDIR)/aws2jsn-dbg $(BINDIR)/aws2jsn-rel

aws_prof: mkdir $(BINDIR)/aws2jsn-prof

$(PREFIX_AWS)_TEST_PARSE_SRC = $(AWS2JSN_CMN_SRC) $(PREFIX_AWS)test_parse.cpp
$(PREFIX_AWS)_TEST_FLEX_SRC = $(PREFIX_AWS)test_flex.cpp $(PREFIX_AWS)scanner.cpp

$(PREFIX_AWS)_TEST_PARSE_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$($(PREFIX_AWS)_TEST_PARSE_SRC)))
$(PREFIX_AWS)_TEST_PARSE_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$($(PREFIX_AWS)_TEST_PARSE_SRC)))

$(PREFIX_AWS)_TEST_FLEX_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$($(PREFIX_AWS)_TEST_FLEX_SRC)))
$(PREFIX_AWS)_TEST_FLEX_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$($(PREFIX_AWS)_TEST_FLEX_SRC)))

$(BINDIR)/$(PREFIX_AWS)test_parse-dbg: $($(PREFIX_AWS)_TEST_PARSE_DBG_OBJ)
	$(LD) -o $@ $^ -lgtest -lpthread -L$(JWT_TOOL)/lib -lncbi-json-dbg -lncbi-secure-dbg -lutf8proc -lmbedcrypto

$(BINDIR)/$(PREFIX_AWS)test_parse-rel: $($(PREFIX_AWS)_TEST_PARSE_REL_OBJ)
	$(LD) -o $@ $^ -lgtest -lpthread -L$(JWT_TOOL)/lib -lncbi-json-rel -lncbi-secure-rel -lutf8proc -lmbedcrypto

$(BINDIR)/$(PREFIX_AWS)test_flex-dbg: $($(PREFIX_AWS)_TEST_FLEX_DBG_OBJ)
	$(LD) -o $@ $^ -lgtest -lpthread

$(BINDIR)/$(PREFIX_AWS)test_flex-rel: $($(PREFIX_AWS)_TEST_FLEX_REL_OBJ)
	$(LD) -o $@ $^ -lgtest -lpthread

#run_$(PREFIX_AWS)parse_test: aws run_test_support run_$(PREFIX_AWS)flex_test $(BINDIR)/$(PREFIX_AWS)test_parse-dbg $(BINDIR)/$(PREFIX_AWS)test_parse-rel
#	$(BINDIR)/$(PREFIX_AWS)test_parse-dbg
#	$(BINDIR)/$(PREFIX_AWS)test_parse-rel

run_$(PREFIX_AWS)parse_test-dbg: aws run_test_support run_$(PREFIX_AWS)flex_test $(BINDIR)/$(PREFIX_AWS)test_parse-dbg
	$(BINDIR)/$(PREFIX_AWS)test_parse-dbg

run_$(PREFIX_AWS)parse_test_rel: aws run_test_support run_$(PREFIX_AWS)flex_test $(BINDIR)/$(PREFIX_AWS)test_parse-rel
	$(BINDIR)/$(PREFIX_AWS)test_parse-rel

run_$(PREFIX_AWS)parse_test:
	$(BINDIR)/$(PREFIX_AWS)test_parse-dbg
	$(BINDIR)/$(PREFIX_AWS)test_parse-rel

run_$(PREFIX_AWS)flex_test: aws $(BINDIR)/$(PREFIX_AWS)test_flex-dbg $(BINDIR)/$(PREFIX_AWS)test_flex-rel
	$(BINDIR)/$(PREFIX_AWS)test_flex-dbg
	$(BINDIR)/$(PREFIX_AWS)test_flex-rel

.PHONY: aws aws_prof run_$(PREFIX_AWS)parse_test run_$(PREFIX_AWS)flex_test

#################################################
# stand alone OP parser

OP2JSN_CMN_SRC = \
	$(COMMON_SRC) \
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
	$(LD) -o $@ $^ -L$(JWT_TOOL)/lib -lncbi-json-dbg -lncbi-secure-dbg $(LDFLAGS)

$(BINDIR)/op2jsn-rel: $(OP2JSN_REL_OBJ) $(REL_TOOL_OBJ)
	$(LD) -o $@ $^ -L$(JWT_TOOL)/lib -lncbi-json-rel -lncbi-secure-rel $(LDFLAGS)

$(BINDIR)/op2jsn-prof: $(OP2JSN_PROF_OBJ) $(DBG_TOOL_OBJ)
	$(LD) -o $@ $^ $(GPROFFLAGS) -L$(JWT_TOOL)/lib -lncbi-json-rel -lncbi-secure-rel $(LDFLAGS)

op: mkdir $(BINDIR)/op2jsn-dbg $(BINDIR)/op2jsn-rel

op_prof: mkdir $(BINDIR)/op2jsn-prof

$(PREFIX_OP)_TEST_PARSE_SRC = $(OP2JSN_CMN_SRC) $(PREFIX_OP)test_parse.cpp
$(PREFIX_OP)_TEST_FLEX_SRC = $(PREFIX_OP)test_flex.cpp $(PREFIX_OP)scanner.cpp

$(PREFIX_OP)_TEST_PARSE_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$($(PREFIX_OP)_TEST_PARSE_SRC)))
$(PREFIX_OP)_TEST_PARSE_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$($(PREFIX_OP)_TEST_PARSE_SRC)))

$(PREFIX_OP)_TEST_FLEX_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$($(PREFIX_OP)_TEST_FLEX_SRC)))
$(PREFIX_OP)_TEST_FLEX_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$($(PREFIX_OP)_TEST_FLEX_SRC)))

$(BINDIR)/$(PREFIX_OP)test_parse-dbg: $($(PREFIX_OP)_TEST_PARSE_DBG_OBJ)
	$(LD) -o $@ $^ -lgtest -lpthread -L$(JWT_TOOL)/lib -lncbi-json-dbg -lncbi-secure-dbg -lutf8proc -lmbedcrypto

$(BINDIR)/$(PREFIX_OP)test_parse-rel: $($(PREFIX_OP)_TEST_PARSE_REL_OBJ)
	$(LD) -o $@ $^ -lgtest -lpthread -L$(JWT_TOOL)/lib -lncbi-json-rel -lncbi-secure-rel -lutf8proc -lmbedcrypto

$(BINDIR)/$(PREFIX_OP)test_flex-dbg: $($(PREFIX_OP)_TEST_FLEX_DBG_OBJ)
	$(LD) -o $@ $^ -lgtest -lpthread

$(BINDIR)/$(PREFIX_OP)test_flex-rel: $($(PREFIX_OP)_TEST_FLEX_REL_OBJ)
	$(LD) -o $@ $^ -lgtest -lpthread

run_$(PREFIX_OP)parse_test: op run_test_support run_$(PREFIX_OP)flex_test $(BINDIR)/$(PREFIX_OP)test_parse-dbg $(BINDIR)/$(PREFIX_OP)test_parse-rel
	$(BINDIR)/$(PREFIX_OP)test_parse-dbg
	$(BINDIR)/$(PREFIX_OP)test_parse-rel

run_$(PREFIX_OP)flex_test: op $(BINDIR)/$(PREFIX_OP)test_flex-dbg $(BINDIR)/$(PREFIX_OP)test_flex-rel
	$(BINDIR)/$(PREFIX_OP)test_flex-dbg
	$(BINDIR)/$(PREFIX_OP)test_flex-rel

.PHONY: op op_prof run_$(PREFIX_OP)parse_test run_$(PREFIX_OP)flex_test

#################################################
# stand alone GCP parser

GCP2JSN_CMN_SRC = \
	$(COMMON_SRC) \
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
	$(LD) -o $@ $^ -L$(JWT_TOOL)/lib -lncbi-json-dbg -lncbi-secure-dbg $(LDFLAGS)

$(BINDIR)/gcp2jsn-rel: $(GCP2JSN_REL_OBJ) $(REL_TOOL_OBJ)
	$(LD) -o $@ $^ -L$(JWT_TOOL)/lib -lncbi-json-rel -lncbi-secure-rel $(LDFLAGS)

$(BINDIR)/gcp2jsn-prof: $(GCP2JSN_PROF_OBJ) $(DBG_TOOL_OBJ)
	$(LD) -o $@ $^ $(GPROFFLAGS) -L$(JWT_TOOL)/lib -lncbi-json-rel -lncbi-secure-rel $(LDFLAGS)

gcp: mkdir $(BINDIR)/gcp2jsn-dbg $(BINDIR)/gcp2jsn-rel

gcp_prof: mkdir $(BINDIR)/gcp2jsn-prof

$(PREFIX_GCP)_TEST_PARSE_SRC = $(GCP2JSN_CMN_SRC) $(PREFIX_GCP)test_parse.cpp
$(PREFIX_GCP)_TEST_FLEX_SRC = $(PREFIX_GCP)test_flex.cpp $(PREFIX_GCP)scanner.cpp

$(PREFIX_GCP)_TEST_PARSE_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$($(PREFIX_GCP)_TEST_PARSE_SRC)))
$(PREFIX_GCP)_TEST_PARSE_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$($(PREFIX_GCP)_TEST_PARSE_SRC)))

$(PREFIX_GCP)_TEST_FLEX_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$($(PREFIX_GCP)_TEST_FLEX_SRC)))
$(PREFIX_GCP)_TEST_FLEX_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$($(PREFIX_GCP)_TEST_FLEX_SRC)))

$(BINDIR)/$(PREFIX_GCP)test_parse-dbg: $($(PREFIX_GCP)_TEST_PARSE_DBG_OBJ)
	$(LD) -o $@ $^ -lgtest -lpthread -L$(JWT_TOOL)/lib -lncbi-json-dbg -lncbi-secure-dbg -lutf8proc -lmbedcrypto

$(BINDIR)/$(PREFIX_GCP)test_parse-rel: $($(PREFIX_GCP)_TEST_PARSE_REL_OBJ)
	$(LD) -o $@ $^ -lgtest -lpthread -L$(JWT_TOOL)/lib -lncbi-json-rel -lncbi-secure-rel -lutf8proc -lmbedcrypto

$(BINDIR)/$(PREFIX_GCP)test_flex-dbg: $($(PREFIX_GCP)_TEST_FLEX_DBG_OBJ)
	$(LD) -o $@ $^ -lgtest -lpthread

$(BINDIR)/$(PREFIX_GCP)test_flex-rel: $($(PREFIX_GCP)_TEST_FLEX_REL_OBJ)
	$(LD) -o $@ $^ -lgtest -lpthread

run_$(PREFIX_GCP)parse_test: gcp run_test_support run_$(PREFIX_GCP)flex_test $(BINDIR)/$(PREFIX_GCP)test_parse-dbg $(BINDIR)/$(PREFIX_GCP)test_parse-rel
	$(BINDIR)/$(PREFIX_GCP)test_parse-dbg
	$(BINDIR)/$(PREFIX_GCP)test_parse-rel

run_$(PREFIX_GCP)flex_test: gcp $(BINDIR)/$(PREFIX_GCP)test_flex-dbg $(BINDIR)/$(PREFIX_GCP)test_flex-rel
	$(BINDIR)/$(PREFIX_GCP)test_flex-dbg
	$(BINDIR)/$(PREFIX_GCP)test_flex-rel

.PHONY: gcp gcp_prof run_$(PREFIX_GCP)parse_test run_$(PREFIX_GCP)flex_test

#################################################
# stand alone TW parser

TW2JSN_CMN_SRC = \
	$(COMMON_SRC) \
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
	$(LD) -o $@ $^ -L$(JWT_TOOL)/lib -lncbi-json-dbg -lncbi-secure-dbg $(LDFLAGS)

$(BINDIR)/tw2jsn-rel: $(TW2JSN_REL_OBJ) $(REL_TOOL_OBJ)
	$(LD) -o $@ $^ -L$(JWT_TOOL)/lib -lncbi-json-rel -lncbi-secure-rel-no-busy $(LDFLAGS)

$(BINDIR)/tw2jsn-prof: $(TW2JSN_PROF_OBJ) $(DBG_TOOL_OBJ)
	$(LD) -o $@ $^ $(GPROFFLAGS) -L$(JWT_TOOL)/lib -lncbi-json-rel -lncbi-secure-rel-no-busy $(LDFLAGS)

tw: mkdir $(BINDIR)/tw2jsn-dbg $(BINDIR)/tw2jsn-rel

tw_prof: mkdir $(BINDIR)/tw2jsn-prof

$(PREFIX_TW)_TEST_PARSE_SRC = $(TW2JSN_CMN_SRC) $(PREFIX_TW)test_parse.cpp
$(PREFIX_TW)_TEST_FLEX_SRC = $(PREFIX_TW)test_flex.cpp $(PREFIX_TW)scanner.cpp

$(PREFIX_TW)_TEST_PARSE_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$($(PREFIX_TW)_TEST_PARSE_SRC)))
$(PREFIX_TW)_TEST_PARSE_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$($(PREFIX_TW)_TEST_PARSE_SRC)))

$(PREFIX_TW)_TEST_FLEX_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$($(PREFIX_TW)_TEST_FLEX_SRC)))
$(PREFIX_TW)_TEST_FLEX_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$($(PREFIX_TW)_TEST_FLEX_SRC)))

$(BINDIR)/$(PREFIX_TW)test_parse-dbg: $($(PREFIX_TW)_TEST_PARSE_DBG_OBJ)
	$(LD) -o $@ $^ -lgtest -lpthread -L$(JWT_TOOL)/lib -lncbi-json-dbg -lncbi-secure-dbg -lutf8proc -lmbedcrypto

$(BINDIR)/$(PREFIX_TW)test_parse-rel: $($(PREFIX_TW)_TEST_PARSE_REL_OBJ)
	$(LD) -o $@ $^ -lgtest -lpthread -L$(JWT_TOOL)/lib -lncbi-json-rel -lncbi-secure-rel -lutf8proc -lmbedcrypto

$(BINDIR)/$(PREFIX_TW)test_flex-dbg: $($(PREFIX_TW)_TEST_FLEX_DBG_OBJ)
	$(LD) -o $@ $^ -lgtest -lpthread

$(BINDIR)/$(PREFIX_TW)test_flex-rel: $($(PREFIX_TW)_TEST_FLEX_REL_OBJ)
	$(LD) -o $@ $^ -lgtest -lpthread

run_$(PREFIX_TW)parse_test: tw run_test_support run_$(PREFIX_TW)flex_test $(BINDIR)/$(PREFIX_TW)test_parse-dbg $(BINDIR)/$(PREFIX_TW)test_parse-rel
	$(BINDIR)/$(PREFIX_TW)test_parse-dbg
	$(BINDIR)/$(PREFIX_TW)test_parse-rel

run_$(PREFIX_TW)flex_test: tw $(BINDIR)/$(PREFIX_TW)test_flex-dbg $(BINDIR)/$(PREFIX_TW)test_flex-rel
	$(BINDIR)/$(PREFIX_TW)test_flex-dbg
	$(BINDIR)/$(PREFIX_TW)test_flex-rel

.PHONY: tw tw_prof run_$(PREFIX_TW)parse_test run_$(PREFIX_TW)flex_test

#################################################
# acceptance

AWS2JSON = $(BINDIR)/aws2jsn-dbg
OP2JSON = $(BINDIR)/op2jsn-dbg
GCP2JSON = $(BINDIR)/gcp2jsn-dbg
TW2JSON = $(BINDIR)/tw2jsn-dbg

jq:
	jq -e . ./expected/*/*.jsonl > /dev/null

test: jq all run_$(PREFIX_AWS)parse_test run_$(PREFIX_GCP)parse_test run_$(PREFIX_OP)parse_test run_$(PREFIX_TW)parse_test
	mkdir -p  actual/$(PREFIX_AWS)testlines
	cat data/aws_testlines.log | $(AWS2JSON) actual/$(PREFIX_AWS)testlines/aws >./actual/$(PREFIX_AWS)testlines/stdout 2>./actual/$(PREFIX_AWS)testlines/stderr \
						&& diff ./expected/$(PREFIX_AWS)testlines ./actual/$(PREFIX_AWS)testlines
	@ echo "test_aws passed"
	@ rm -rf actual/$(PREFIX_AWS)*
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

vg_aws : $(BINDIR)/aws2jsn-$(BUILD) $(BINDIR)/$(PREFIX_AWS)test_flex-$(BUILD) $(BINDIR)/$(PREFIX_AWS)test_parse-$(BUILD)
	valgrind $(VG_OPTIONS) $(BINDIR)/$(PREFIX_AWS)test_flex-$(BUILD)
	valgrind $(VG_OPTIONS) $(BINDIR)/$(PREFIX_AWS)test_parse-$(BUILD)
	cat data/aws_testlines.log | valgrind $(VG_OPTIONS) $(BINDIR)/aws2jsn-$(BUILD) actual/aws

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
	rm -rf fuzz/

run_fuzz: run_aws_fuzz run_gcp_fuzz run_op_fuzz run_tw_fuzz

.phony: clean_fuzz run_fuzz

#-------------------------------------------------------------------------------
# fuzz testing AWS
#
AWS_FUZZ_SRC = \
	$(AWS2JSN_CMN_SRC) \
	$(PREFIX_AWS)fuzz.cpp

AWS_FUZZ_OBJ = \
	$(addprefix $(OBJDIR)/,$(subst .cpp,.fuzz.o,$(AWS_FUZZ_SRC)))

$(BINDIR)/aws-fuzz: aws $(AWS_FUZZ_OBJ)
	clang++ -g -o $@ $(AWS_FUZZ_OBJ) $(FUZZ_OPT) -L$(JWT_TOOL)/lib -lncbi-json-dbg -lncbi-secure-dbg $(LDFLAGS)

#AWS_FUZZ_DICT ?= -dict=fuzz/aws-dict

run_aws_fuzz: $(BINDIR)/aws-fuzz
	 mkdir -p fuzz/aws-corpus fuzz/aws-seeds
	 cp data/aws_testlines.log fuzz/aws-seeds/
	 $^ $(AWS_FUZZ_DICT) -runs=$(FUZZ_RUNS) fuzz/aws-corpus fuzz/aws-seeds

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
	clang++ -g -o $@ $(GCP_FUZZ_OBJ) $(FUZZ_OPT) -L$(JWT_TOOL)/lib -lncbi-json-dbg -lncbi-secure-dbg $(LDFLAGS)

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
	clang++ -g -o $@ $(OP_FUZZ_OBJ) $(FUZZ_OPT) -L$(JWT_TOOL)/lib -lncbi-json-dbg -lncbi-secure-dbg $(LDFLAGS)

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
	clang++ -g -o $@ $(TW_FUZZ_OBJ) $(FUZZ_OPT) -L$(JWT_TOOL)/lib -lncbi-json-dbg -lncbi-secure-dbg $(LDFLAGS)

#TW_FUZZ_DICT ?= -dict=fuzz/op-dict

run_tw_fuzz: $(BINDIR)/tw-fuzz
	 mkdir -p fuzz/tw-corpus fuzz/tw-seeds
	 cp data/tw_testlines.log fuzz/tw-seeds/
	 $^ $(TW_FUZZ_DICT) -runs=$(FUZZ_RUNS) fuzz/tw-corpus fuzz/tw-seeds/

.phony: run_tw_fuzz
