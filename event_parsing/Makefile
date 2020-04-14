# ----------------------------------------------------------------------
#
# can be called:
#
# make ................................... handles yesterday
# make YEAR="2019" MONTH="04" DAY="01" ... handles this date
# make MONTH="04" DAY="01" ............... handles this date ( year of yesterday )
# make DAY="01" .......................... handles this date ( year/month of yesterday )
# make AGO="2" ........................... handles day before yesterday
#
# make -j 12 ............................. max 12 parallel reduce-steps
#
# ----------------------------------------------------------------------

#let us test if the basic source-directory does exist
SRC0  := /panfs/pan1/applog_db_tmp/database/logarchive/ftp.http/local_archive/
SRC0E := $(shell if [ -d $(SRC0) ]; then echo "exists"; fi )
ifeq ($(SRC0E),)
    $(error source-directory $(SRC0) does not exist)
endif

#let us test if the basic destination-directory does exist
DST0  := /panfs/traces01/sra_review/scratch/$(shell whoami)/
DST0E := $(shell if [ -d $(DST0) ]; then echo "exists"; fi )
ifeq ($(DST0E),)
    $(error destination-directory $(DST0) does not exist)
endif

#let us test if we already have or can create this sub-directory
# >>>>>>>>>> replace with mikes_applogs <<<<<<<<<<
DST1 := $(DST0)mikes_applogs/
DST1E := $(shell if [ -d $(DST1) ]; then echo "exists"; fi )
ifeq ($(DST1E),)
    $(shell mkdir -p $(DST1) )
    DST1E := $(shell if [ -d $(DST1) ]; then echo "exists"; fi )    
endif
ifeq ($(DST1E),)
    $(error creating destination-directory $(DST1))
endif

#let us test what parameters are passed in, and fill in what is missing
AGO   := 1
YEAR  :=
MONTH :=
DAY   :=

ifeq ($(YEAR),)
    YEAR := $(shell date --date='$(AGO) days ago' +%Y)
endif

ifeq ($(MONTH),)
    MONTH := $(shell date --date='$(AGO) days ago' +%m)
endif

ifeq ($(DAY),)
    DAY := $(shell date --date='$(AGO) days ago' +%d)
endif

#let us test if the daily source-directory does exist
SRC1 := $(SRC0)$(YEAR)/$(MONTH)/$(DAY)/
SRC1E := $(shell if [ -d $(SRC1) ]; then echo "exists"; fi )
ifeq ($(SRC1E),)
    $(error source-directory $(SRC1) does not exist)
endif

#let us test if the daily desination-directory does exist
#...and create it ( if it does not exist )
DST2 := $(DST1)$(YEAR)_$(MONTH)/$(YEAR)_$(MONTH)_$(DAY)/
DST2E := $(shell if [ -d $(DST2) ]; then echo "exists"; fi )
ifeq ($(DST2E),)
    $(shell mkdir -p $(DST2) )
    DST2E := $(shell if [ -d $(DST2) ]; then echo "exists"; fi )    
endif
ifeq ($(DST2E),)
    $(error creating destination-directory $(DST2))
endif

#this is the file containing date and time after transmitting
SESSION_TRANSMIT := $(DST2)sessions.transmit
SESSION_TRANSMITE := $(shell if [ -f $(SESSION_TRANSMIT) ]; then echo "exists"; fi )
ifeq ($(SESSION_TRANSMITE),exists)
    $(error we have already transmitted this data!)
endif

#these are the server-subdirs of SRC1 we are interested in
#for each of these we have to produce a cumulative server-file in DST2
SERVERS := faspgap11 faspgap12 faspgap21 faspgap22 faspgap31 faspgap32 \
 srafiles11 srafiles12 srafiles21 srafiles22 srafiles31 srafiles32

#this list allows to write a rule to produce a server-file for each server
SERVER_FILES := $(foreach server,$(SERVERS),$(DST2)$(server).gz)

#this list allows to write a rule to convert server-files into session-files
SESSION_FILES := $(foreach server,$(SERVERS),$(DST2)$(server).session)

#this is the file containing all sessions for this day
SESSION_ALL := $(DST2)sessions.all

MIN_GAP := 30
# >>>>>>>>>> intprod11-new <<<<<<<<<<
SERVER := intprod11
PORT := 20817
CGI := sess_start_tsv

.PHONY : clean

#if not given, intermediate files will be deleted!
.PRECIOUS : $(SERVER_FILES) $(SESSION_FILES) $(SESSION_ALL) $(SESSION_TRANSMIT)

#we have to specify that before! all the rules, otherwise it picks the first rule
all : $(SESSION_TRANSMIT)

#the rule how to produce the server-files by concatenating from the source
$(DST2)%.gz : $(SRC1)%
	@echo concatenate $(YEAR)/$(MONTH)/$(DAY) $(notdir $@)
	@cat $^/access*.gz > $@

#the rule how to produce the session-files by converting from the server-file
$(DST2)%.session : $(DST2)%.gz | zreduce2
	@echo reduce $(YEAR)/$(MONTH)/$(DAY) $(notdir $@)
	@zcat $^ | ./zreduce2 stdin $@ --min_gap=$(MIN_GAP)

#the rule to produce the sessions.all file for the day
$(SESSION_ALL) : $(SESSION_FILES) | zsort
	@echo combine to $(YEAR)/$(MONTH)/$(DAY) $(notdir $@)
	@cat $^ | ./zsort > $@

#the rule to send the sessions.all file to the server
$(SESSION_TRANSMIT) : $(SESSION_ALL) | zpost
	@cat $(SESSION_ALL) | ./zpost --server=$(SERVER) --port=$(PORT) --cgi=$(CGI)
	date >> $(SESSION_TRANSMIT)
	@echo DONE!

#that makes it dependent on ncib-infrastructure!
CPP := /opt/ncbi/gcc/7.3.0/bin/gcc

#all we need to build the zreduce-tool
zreduce_src := src/log_event.c src/log_session.c src/t_str.c src/zreduce2.c
zreduce_hdr := src/log_event.h src/log_session.h src/t_str.h
zreduce2 : $(zreduce_src) $(zreduce.hdr)
	$(CPP) $(zreduce_src) -g -O3 -lm -o $@

#all we need to build the zsort-tool
zsort_src := src/zsort.cpp
zsort_hdr := src/session.hpp
zsort : $(zsort_src) $(zsort.hdr)
	$(CPP) $(zsort_src) -Wall -O3 -std=c++17 -lstdc++ -o $@

#all we need to build the zpost-tool
zpost_src := src/zpost.cpp
zpost_hdr := src/session.hpp src/settings.hpp
zpost : $(zpost_src) $(zpost.hdr)
	$(CPP) $(zpost_src) -Wall -O3 -std=c++17 -lstdc++ -lcurl -o $@

clean :
	@rm -rf zreduce zsort zpost 
