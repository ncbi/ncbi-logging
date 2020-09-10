.PHONY: clean all run prof

BINDIR = bin
LIBDIR = lib
OBJDIR = obj

CC := gcc -c
CPP := g++ -c
LD := g++
CXXFLAGS += -I . -MMD
GPROFFLAGS = -pg -O3
#FLEXFLAGS = -8 --fast
FLEXFLAGS = -8 --align

PREFIX_OP_V2 := op_v2_
PREFIX_AWS_V2 := aws_v2_
PREFIX_GCP_V2 := gcp_v2_
PREFIX_TW_V2 := tw_v2_

all: v2
	@echo "done"

#do not rm XXX_parser.*/XXX_scanner.*! this would deleted *.l and *.y too!
clean:
	rm -rf $(LIBDIR) $(BINDIR) $(OBJDIR)
	rm -f $(PREFIX_OP_V2)parser.hpp $(PREFIX_OP_V2)parser.cpp
	rm -f $(PREFIX_OP_V2)scanner.hpp $(PREFIX_OP_V2)scanner.cpp
	rm -f $(PREFIX_OP_V2)parser.output
	rm -f $(PREFIX_AWS_V2)parser.hpp $(PREFIX_AWS_V2)parser.cpp
	rm -f $(PREFIX_AWS_V2)scanner.hpp $(PREFIX_AWS_V2)scanner.cpp
	rm -f $(PREFIX_AWS_V2)parser.output
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

# instrumented for fuzzing
FUZZ_OPT = -fsanitize=fuzzer,address,signed-integer-overflow -fprofile-instr-generate -fcoverage-mapping
#FUZZ_OPT = -fsanitize=fuzzer,signed-integer-overflow -fprofile-instr-generate -fcoverage-mapping
$(OBJDIR)/%.fuzz.o: %.cpp
	clang++ -c $< -o $@ -g $(CXXFLAGS) $(FUZZ_OPT)

-include $(OBJDIR)/*.d

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
# unit tests for formatter/catwriter

TEST_SUPPORT_SRC = \
	Formatters.cpp test_formatter.cpp \
	CatWriters.cpp test_catwriter.cpp \
	Queues.cpp test_queues.cpp \
	LineSplitters.cpp test_linesplitter.cpp \
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
	Queues.cpp \
	LineSplitters.cpp

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
	cat data/aws_testlines.log | $(AWS2JSON) actual/$(PREFIX_AWS_V2)testlines/aws >./actual/$(PREFIX_AWS_V2)testlines/stdout 2>./actual/$(PREFIX_AWS_V2)testlines/stderr \
						&& diff ./expected/$(PREFIX_AWS_V2)testlines ./actual/$(PREFIX_AWS_V2)testlines
	@ echo "test_aws v2 passed"
	@ rm -rf actual/$(PREFIX_AWS_V2)*
	#
	mkdir -p  actual/$(PREFIX_OP_V2)testlines
	cat data/op_testlines.log | $(OP2JSON) actual/$(PREFIX_OP_V2)testlines/op >./actual/$(PREFIX_OP_V2)testlines/stdout 2>./actual/$(PREFIX_OP_V2)testlines/stderr \
						&& diff ./expected/$(PREFIX_OP_V2)testlines ./actual/$(PREFIX_OP_V2)testlines
	@ echo "test_op v2 passed"
	@ rm -rf actual/$(PREFIX_OP_V2)*
	#
	mkdir -p  actual/$(PREFIX_GCP_V2)testlines
	cat data/gcp_testlines.log | $(GCP2JSON) actual/$(PREFIX_GCP_V2)testlines/gcp >./actual/$(PREFIX_GCP_V2)testlines/stdout 2>./actual/$(PREFIX_GCP_V2)testlines/stderr \
						&& diff ./expected/$(PREFIX_GCP_V2)testlines ./actual/$(PREFIX_GCP_V2)testlines
	@ echo "test_gcp v2 passed"
	@ rm -rf actual/$(PREFIX_GCP_V2)*
	#
	mkdir -p  actual/$(PREFIX_TW_V2)testlines
	cat data/tw_testlines.log | $(TW2JSON) actual/$(PREFIX_TW_V2)testlines/tw >./actual/$(PREFIX_TW_V2)testlines/stdout 2>./actual/$(PREFIX_TW_V2)testlines/stderr \
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

vg_op : $(BINDIR)/op2jsn-$(BUILD) $(BINDIR)/$(PREFIX_OP_V2)test_flex-$(BUILD) $(BINDIR)/$(PREFIX_OP_V2)test_parse-$(BUILD)
	valgrind $(VG_OPTIONS) $(BINDIR)/$(PREFIX_OP_V2)test_flex-$(BUILD)
	valgrind $(VG_OPTIONS) $(BINDIR)/$(PREFIX_OP_V2)test_parse-$(BUILD)
	cat data/op_testlines.log | valgrind $(VG_OPTIONS) $(BINDIR)/op2jsn-$(BUILD) actual/op

vg_gcp : $(BINDIR)/gcp2jsn-$(BUILD) $(BINDIR)/$(PREFIX_GCP_V2)test_flex-$(BUILD) $(BINDIR)/$(PREFIX_GCP_V2)test_parse-$(BUILD)
	valgrind $(VG_OPTIONS) $(BINDIR)/$(PREFIX_GCP_V2)test_flex-$(BUILD)
	valgrind $(VG_OPTIONS) $(BINDIR)/$(PREFIX_GCP_V2)test_parse-$(BUILD)
	cat data/gcp_testlines.log | valgrind $(VG_OPTIONS) $(BINDIR)/gcp2jsn-$(BUILD) actual/gcp

vg_aws : $(BINDIR)/aws2jsn-$(BUILD) $(BINDIR)/$(PREFIX_AWS_V2)test_flex-$(BUILD) $(BINDIR)/$(PREFIX_AWS_V2)test_parse-$(BUILD)
	valgrind $(VG_OPTIONS) $(BINDIR)/$(PREFIX_AWS_V2)test_flex-$(BUILD)
	valgrind $(VG_OPTIONS) $(BINDIR)/$(PREFIX_AWS_V2)test_parse-$(BUILD)
	cat data/aws_testlines.log | valgrind $(VG_OPTIONS) $(BINDIR)/aws2jsn-$(BUILD) actual/aws

vg_tw : $(BINDIR)/tw2jsn-$(BUILD) $(BINDIR)/$(PREFIX_TW_V2)test_flex-$(BUILD) $(BINDIR)/$(PREFIX_TW_V2)test_parse-$(BUILD)
	valgrind $(VG_OPTIONS) $(BINDIR)/$(PREFIX_TW_V2)test_flex-$(BUILD)
	valgrind $(VG_OPTIONS) $(BINDIR)/$(PREFIX_TW_V2)test_parse-$(BUILD)
	cat data/tw_testlines.log | valgrind $(VG_OPTIONS) $(BINDIR)/tw2jsn-$(BUILD) actual/tw

vg_all : vg_op vg_gcp vg_aws vg_tw

.PHONY: aws_op vg_gcp vg_aws vg_tw vg_all

#-------------------------------------------------------------------------------
# fuzz testing
clean_fuzz:
	rm -rf fuzz/

#-------------------------------------------------------------------------------
# fuzz testing AWS
#
AWS_FUZZ_SRC = \
	$(AWS2JSN_CMN_SRC) \
	$(PREFIX_AWS_V2)fuzz.cpp

AWS_FUZZ_OBJ = \
	$(addprefix $(OBJDIR)/,$(subst .cpp,.fuzz.o,$(AWS_FUZZ_SRC)))

$(BINDIR)/aws-fuzz: aws $(AWS_FUZZ_OBJ)
	clang++ -g -o $@ $(AWS_FUZZ_OBJ) $(FUZZ_OPT) -L$(JWT_TOOL)/lib -lncbi-json-dbg -lncbi-secure-dbg -lutf8proc -lmbedcrypto -lpthread

#AWS_FUZZ_DICT ?= -dict=fuzz/aws-dict
AWS_RUNS ?= 25000

run_aws_fuzz: $(BINDIR)/aws-fuzz
	 mkdir -p fuzz/aws-corpus fuzz/aws-seeds
	 cp data/aws_testlines.log fuzz/aws-seeds/
	 $^ $(AWS_FUZZ_DICT) -runs=$(AWS_RUNS) fuzz/aws-corpus fuzz/aws-seeds

.phony: run_aws_fuzz

#-------------------------------------------------------------------------------
# fuzz testing GCP
#
GCP_FUZZ_SRC = \
	$(GCP2JSN_CMN_SRC) \
	$(PREFIX_GCP_V2)fuzz.cpp

GCP_FUZZ_OBJ = \
	$(addprefix $(OBJDIR)/,$(subst .cpp,.fuzz.o,$(GCP_FUZZ_SRC)))

$(BINDIR)/gcp-fuzz: gcp $(GCP_FUZZ_OBJ)
	clang++ -g -o $@ $(GCP_FUZZ_OBJ) $(FUZZ_OPT) -L$(JWT_TOOL)/lib -lncbi-json-dbg -lncbi-secure-dbg -lutf8proc -lmbedcrypto -lpthread

#GCP_FUZZ_DICT ?= -dict=fuzz/gcp-dict
GCP_RUNS ?= 25000

run_gcp_fuzz: $(BINDIR)/gcp-fuzz
	 mkdir -p fuzz/gcp-corpus fuzz/gcp-seeds
	 cp data/gcp_testlines.log fuzz/gcp-seeds/
	 $^ $(GCP_FUZZ_DICT) -runs=$(GCP_RUNS) fuzz/gcp-corpus fuzz/gcp-seeds/

.phony: run_gcp_fuzz

#-------------------------------------------------------------------------------
# fuzz testing OP
#
OP_FUZZ_SRC = \
	$(OP2JSN_CMN_SRC) \
	$(PREFIX_OP_V2)fuzz.cpp

OP_FUZZ_OBJ = \
	$(addprefix $(OBJDIR)/,$(subst .cpp,.fuzz.o,$(OP_FUZZ_SRC)))

$(BINDIR)/op-fuzz: op $(OP_FUZZ_OBJ)
	clang++ -g -o $@ $(OP_FUZZ_OBJ) $(FUZZ_OPT) -L$(JWT_TOOL)/lib -lncbi-json-dbg -lncbi-secure-dbg -lutf8proc -lmbedcrypto -lpthread

#OP_FUZZ_DICT ?= -dict=fuzz/op-dict
OP_RUNS ?= 25000

run_op_fuzz: $(BINDIR)/op-fuzz
	 mkdir -p fuzz/op-corpus fuzz/op-seeds
	 cp data/op_testlines.log fuzz/op-seeds/
	 $^ $(OP_FUZZ_DICT) -runs=$(OP_RUNS) fuzz/op-corpus fuzz/op-seeds/

.phony: run_op_fuzz

#-------------------------------------------------------------------------------
# fuzz testing TW
#
TW_FUZZ_SRC = \
	$(TW2JSN_CMN_SRC) \
	$(PREFIX_TW_V2)fuzz.cpp

TW_FUZZ_OBJ = \
	$(addprefix $(OBJDIR)/,$(subst .cpp,.fuzz.o,$(TW_FUZZ_SRC)))

$(BINDIR)/tw-fuzz: tw $(TW_FUZZ_OBJ)
	clang++ -g -o $@ $(TW_FUZZ_OBJ) $(FUZZ_OPT) -L$(JWT_TOOL)/lib -lncbi-json-dbg -lncbi-secure-dbg -lutf8proc -lmbedcrypto -lpthread

#TW_FUZZ_DICT ?= -dict=fuzz/op-dict
TW_RUNS ?= 25000

run_tw_fuzz: $(BINDIR)/tw-fuzz
	 mkdir -p fuzz/tw-corpus fuzz/tw-seeds
	 cp data/tw_testlines.log fuzz/tw-seeds/
	 $^ $(TW_FUZZ_DICT) -runs=$(TW_RUNS) fuzz/tw-corpus fuzz/tw-seeds/

.phony: run_tw_fuzz

