BINDIR = ../bin
OBJDIR = ../obj

CC := gcc -c
CPP := g++ -c
LD := g++
CXXFLAGS += -I . -MMD
GPROFFLAGS = -pg -O3

all: cmd support

test: run_test_support

clean: clean_support

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

-include $(OBJDIR)/*.d

.phony: all test clean mkdir

#################################################
# Specify location of ncbi-vdb3/jwt-tool in JWT_TOOL if different
JWT_TOOL ?= ../../../ncbi-vdb3/jwt-tool

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

cmd: $(DBG_CMD_OBJ) $(REL_CMD_OBJ)

.phony: cmd

#################################################
# utility modules

COMMON_SRC = \
	CatWriters.cpp \
	Formatters.cpp \
	ReceiverInterface.cpp \
	Queues.cpp \
	LineSplitters.cpp \
	Tool.cpp

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

support: $(TEST_SUPPORT_DBG_OBJ) $(TEST_SUPPORT_REL_OBJ)

$(BINDIR)/test_support-dbg: $(TEST_SUPPORT_DBG_OBJ)
	$(LD) -o $@ $^ $(DBG_CMD_OBJ) -L$(JWT_TOOL)/lib -lncbi-json-dbg -lncbi-secure-dbg -lutf8proc -lmbedcrypto -lgtest -lpthread

$(BINDIR)/test_support-rel: $(TEST_SUPPORT_REL_OBJ)
	$(LD) -o $@ $^ $(REL_CMD_OBJ) -L$(JWT_TOOL)/lib -lncbi-json-rel -lncbi-secure-rel -lutf8proc -lmbedcrypto -lgtest -lpthread

clean_support:
	rm -rf $(TEST_SUPPORT_DBG_OBJ) $(TEST_SUPPORT_REL_OBJ) $(BINDIR)/test_support-dbg $(BINDIR)/test_support-rel

run_test_support: mkdir $(BINDIR)/test_support-dbg $(BINDIR)/test_support-rel
	$(BINDIR)/test_support-dbg
	$(BINDIR)/test_support-rel

.PHONY: support clean_support run_test_support

#-------------------------------------------------------------------------------
# fuzz testing: TBD

# FUZZ_THREADS = 2
# FUZZ_RUNS ?= 5000

# FUZZ_OPT = -DTHREAD_NUM=$(FUZZ_THREADS) -fsanitize=fuzzer,address,signed-integer-overflow -fprofile-instr-generate -fcoverage-mapping

# $(OBJDIR)/%.fuzz.o: %.cpp
# 	clang++ -c $< -o $@ -g $(CXXFLAGS) $(FUZZ_OPT)

# clean_fuzz:
# 	#TBD rm -rf fuzz/

# run_fuzz:
# 	$TBD

# .phony: clean_fuzz run_fuzz
