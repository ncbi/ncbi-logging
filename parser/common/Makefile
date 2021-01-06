# COMMON_SRC
include ../Makefile.set

all: cmd common

test: cmn_test

clean: cmn_clean

prof: cmn_prof

mkdir:
	@ mkdir -p $(BINDIR) $(OBJDIR)
	@ echo "mkdir run"

.phony: all test clean mkdir prof

#################################################
# VDB3 Command line parsing
CMD_SRC_LOCATION = $(JWT_TOOL)/tool
CMD_SRC = $(CMD_SRC_LOCATION)/cmdline.cpp

CMD_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$(notdir $(CMD_SRC))))
CMD_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$(notdir $(CMD_SRC))))
CMD_PROF_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.prof.o,$(notdir $(CMD_SRC))))

$(OBJDIR)/%.dbg.o: $(CMD_SRC_LOCATION)/%.cpp
	$(CPP) $(CXXFLAGS) -g $< -o $(OBJDIR)/$(*F).dbg.o

$(OBJDIR)/%.rel.o: $(CMD_SRC_LOCATION)/%.cpp
	$(CPP) $(CXXFLAGS) -O3 $< -o $(OBJDIR)/$(*F).rel.o

$(OBJDIR)/%.prof.o: $(CMD_SRC_LOCATION)/%.cpp
	$(CPP) $(CXXFLAGS) -O3 $< -o $(OBJDIR)/$(*F).prof.o

cmd: mkdir $(CMD_DBG_OBJ) $(CMD_REL_OBJ) $(CMD_PROF_OBJ)

.phony: cmd

#################################################
# post-process parser-dependencies

$(OBJDIR)/JWT_Interface.dbg.o $(OBJDIR)/JWT_Interface.rel.o $(OBJDIR)/JWT_Interface.prof.o : jwt_parser.hpp
$(OBJDIR)/jwt_parser.dbg.o $(OBJDIR)/jwt_parser.rel.o $(OBJDIR)/jwt_parser.prof.o : jwt_parser.hpp
jwt_parser.hpp : jwt_parser.cpp

$(OBJDIR)/AGENT_Interface.dbg.o $(OBJDIR)/AGENT_Interface.rel.o $(OBJDIR)/AGENT_Interface.prof.o : agent_parser.hpp
$(OBJDIR)/agent_parser.dbg.o $(OBJDIR)/agent_parser.rel.o $(OBJDIR)/agent_parser.prof.o : agent_parser.hpp
agent_parser.hpp : agent_parser.cpp

$(OBJDIR)/URL_Interface.dbg.o $(OBJDIR)/URL_Interface.rel.o $(OBJDIR)/URL_Interface.prof.o : url_parser.hpp
$(OBJDIR)/url_parser.dbg.o $(OBJDIR)/url_parser.rel.o $(OBJDIR)/url_parser.prof.o : url_parser.hpp
url_parser.hpp : url_parser.cpp

$(OBJDIR)/CL_PATH_Interface.dbg.o $(OBJDIR)/CL_PATH_Interface.rel.o $(OBJDIR)/CL_PATH_Interface.prof.o : cl_path_parser.hpp
$(OBJDIR)/cl_path_parser.dbg.o $(OBJDIR)/cl_path_parser.rel.o $(OBJDIR)/cl_path_parser.prof.o : cl_path_parser.hpp
cl_path_parser.hpp : cl_path_parser.cpp

.precious: jwt_scanner.cpp   jwt_scanner.hpp   jwt_parser.cpp   jwt_parser.hpp
.precious: agent_scanner.cpp agent_scanner.hpp agent_parser.cpp agent_parser.hpp
.precious: url_scanner.cpp   url_scanner.hpp   url_parser.cpp   url_parser.hpp
.precious: cl_path_scanner.cpp   cl_path_scanner.hppcl_path_parser.cpp   cl_path_parser.hpp

#################################################
# unit tests for formatter/catwriter/etc

TEST_SUPPORT_SRC = $(COMMON_SRC) \
	test_formatter.cpp \
	test_catwriter.cpp \
	test_queues.cpp \
	test_linesplitter.cpp \
	test_jwt_parsing.cpp \
	test_agent_parsing.cpp \
	test_url_flex.cpp \
	test_url_parsing.cpp \
	test_cl_path_flex.cpp \
	test_cl_path_parsing.cpp \
	test_support_main.cpp

$(OBJDIR)/test_url_flex.dbg.o: url_scanner.cpp
$(OBJDIR)/test_url_flex.rel.o: url_scanner.cpp
$(OBJDIR)/test_cl_path_flex.dbg.o: cl_path_scanner.cpp
$(OBJDIR)/test_cl_path_flex.rel.o: cl_path_scanner.cpp

TEST_SUPPORT_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$(TEST_SUPPORT_SRC)))
TEST_SUPPORT_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$(TEST_SUPPORT_SRC)))

ALL_OBJ = $(COMMON_DBG_OBJ) $(COMMON_REL_OBJ) $(COMMON_PROF_OBJ) $(CMD_DBG_OBJ) $(CMD_REL_OBJ) $(CMD_PROF_OBJ) $(TEST_SUPPORT_DBG_OBJ) $(TEST_SUPPORT_REL_OBJ)  $(TEST_SUPPORT_PROF_OBJ)
ALL_D = $(subst .o,.d,$(ALL_OBJ))
-include $(ALL_D)

common: mkdir $(COMMON_DBG_OBJ) $(COMMON_REL_OBJ)

cmn_clean:
	@ rm -rf $(ALL_OBJ) $(ALL_D)
	@ rm -rf jwt_scanner.cpp jwt_scanner.hpp
	@ rm -rf jwt_parser.cpp jwt_parser.hpp
	@ rm -rf agent_scanner.cpp agent_scanner.hpp
	@ rm -rf agent_parser.cpp agent_parser.hpp
	@ rm -rf url_scanner.cpp url_scanner.hpp
	@ rm -rf url_parser.cpp url_parser.hpp
	@ rm -rf cl_path_scanner.cpp cl_path_scanner.hpp
	@ rm -rf cl_path_parser.cpp cl_path_parser.hpp
	@ rm -rf $(BINDIR)/test_common-dbg $(BINDIR)/test_common-rel

cmn_test: mkdir $(BINDIR)/test_common-dbg $(BINDIR)/test_common-rel
	$(BINDIR)/test_common-dbg
	$(BINDIR)/test_common-rel

cmn_prof: mkdir $(COMMON_PROF_OBJ) $(CMD_PROF_OBJ)

$(BINDIR)/test_common-dbg: $(TEST_SUPPORT_DBG_OBJ) $(CMD_DBG_OBJ)
	$(LD) -o $@ $^ -L$(JWT_TOOL)/lib -lncbi-json-dbg -lncbi-secure-dbg -lutf8proc -lmbedcrypto -lgtest -lpthread

$(BINDIR)/test_common-rel: $(TEST_SUPPORT_REL_OBJ) $(CMD_REL_OBJ)
	$(LD) -o $@ $^ -L$(JWT_TOOL)/lib -lncbi-json-rel -lncbi-secure-rel -lutf8proc -lmbedcrypto -lgtest -lpthread

.PHONY: common cmn_clean cmn_test

#-------------------------------------------------------------------------------
VG_OPTIONS := --leak-check=full --error-exitcode=1
VG_BUILD = dbg
#VG_BUILD = rel

vg : $(BINDIR)/test_common-dbg $(BINDIR)/test_common-rel
	valgrind $(VG_OPTIONS) $(BINDIR)/test_common-$(VG_BUILD)

.phony: vg

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
