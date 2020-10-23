PREFIX := err
CPREFIX := ERR

$(CPREFIX)2JSN_CMN_SRC = \
	msg_parser.cpp \
	msg_scanner.cpp \
	MSG_Interface.cpp

$(PREFIX)_TEST_REVERSE_SRC = \
	msg_parser.cpp \
	msg_scanner.cpp \
	MSG_Interface.cpp

include ../Makefile.generic

# ERR-only MSG post-processor

$(OBJDIR)/msg_parser.dbg.o:  	$(OBJDIR)/msg_scanner.dbg.o
$(OBJDIR)/msg_parser.rel.o:  	$(OBJDIR)/msg_scanner.rel.o
$(OBJDIR)/msg_parser.prof.o:  	$(OBJDIR)/msg_scanner.prof.o

MSG_TEST_FLEX_SRC = msg_test_flex.cpp msg_scanner.cpp

MSG_TEST_FLEX_DBG_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.dbg.o,$(MSG_TEST_FLEX_SRC)))
MSG_TEST_FLEX_REL_OBJ = $(addprefix $(OBJDIR)/,$(subst .cpp,.rel.o,$(MSG_TEST_FLEX_SRC)))

$(BINDIR)/msg_test_flex-dbg: $(MSG_TEST_FLEX_DBG_OBJ)
	$(LD) -o $@ $^ -lgtest -lpthread

$(BINDIR)/msg_test_flex-rel: $(MSG_TEST_FLEX_REL_OBJ)
	$(LD) -o $@ $^ -lgtest -lpthread

flex_test: msg_flex_test

msg_flex_test: $(BINDIR)/msg_test_flex-dbg $(BINDIR)/msg_test_flex-rel
	$(BINDIR)/msg_test_flex-dbg
	$(BINDIR)/msg_test_flex-rel

.phony: msg_flex_test