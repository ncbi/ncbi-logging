include ../Makefile.set

all: cmd common

test: cmn_test

clean: cmn_clean

mkdir:
	@ mkdir -p $(BINDIR) $(OBJDIR)
	@ echo "mkdir run"

.phony: all test clean mkdir

#################################################
# VDB3 Command line parsing
CMD_SRC_LOCATION = $(JWT_TOOL)/tool
CMD_SRC = $(CMD_SRC_LOCATION)/cmdline.cpp

CMD_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$(notdir $(CMD_SRC))))
CMD_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$(notdir $(CMD_SRC))))

$(OBJDIR)/%.dbg.o: $(CMD_SRC_LOCATION)/%.cpp
	$(CPP) $(CXXFLAGS) -g $< -o $(OBJDIR)/$(*F).dbg.o

$(OBJDIR)/%.rel.o: $(CMD_SRC_LOCATION)/%.cpp
	$(CPP) $(CXXFLAGS) -O3 $< -o $(OBJDIR)/$(*F).rel.o

cmd: mkdir $(CMD_DBG_OBJ) $(CMD_REL_OBJ)

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

ALL_OBJ = $(CMD_DBG_OBJ) $(CMD_REL_OBJ) $(TEST_SUPPORT_DBG_OBJ) $(TEST_SUPPORT_REL_OBJ)
ALL_D = $(subst .o,.d,$(ALL_OBJ))
-include $(ALL_D)

common: mkdir $(COMMON_DBG_OBJ) $(COMMON_REL_OBJ)

cmn_clean:
	@ rm -rf $(ALL_OBJ) $(ALL_D)
	@ rm -rf $(BINDIR)/test_common-dbg $(BINDIR)/test_common-rel

cmn_test: mkdir $(BINDIR)/test_common-dbg $(BINDIR)/test_common-rel
	$(BINDIR)/test_common-dbg
	$(BINDIR)/test_common-rel

$(BINDIR)/test_common-dbg: $(TEST_SUPPORT_DBG_OBJ) $(CMD_DBG_OBJ)
	$(LD) -o $@ $^ -L$(JWT_TOOL)/lib -lncbi-json-dbg -lncbi-secure-dbg -lutf8proc -lmbedcrypto -lgtest -lpthread

$(BINDIR)/test_common-rel: $(TEST_SUPPORT_REL_OBJ) $(CMD_REL_OBJ)
	$(LD) -o $@ $^ -L$(JWT_TOOL)/lib -lncbi-json-rel -lncbi-secure-rel -lutf8proc -lmbedcrypto -lgtest -lpthread

.PHONY: common cmn_clean cmn_test

#-------------------------------------------------------------------------------
# vg: TBD

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