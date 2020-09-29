
all: cmn aws op gcp tw
	@echo "done"

cmn:
	$(MAKE) -C common

test: test_cmn acc_test

test_cmn:
	$(MAKE) -C common test

cmn_clean:
	$(MAKE) -C common clean

clean: aws_clean op_clean gcp_clean tw_clean cmn_clean
	rm -rf *.output

#################################################
# stand alone AWS parser

aws:
	$(MAKE) -C aws

aws_prof:
	$(MAKE) -C aws prof

aws_test:
	$(MAKE) -C aws test

aws_clean:
	$(MAKE) -C aws clean

.PHONY: aws aws_prof aws_clean aws_test

#################################################
# stand alone GCP parser

gcp:
	$(MAKE) -C gcp

gcp_prof:
	$(MAKE) -C gcp prof

gcp_test:
	$(MAKE) -C gcp test

gcp_clean:
	$(MAKE) -C gcp clean

.PHONY: gcp gcp_prof gcp_clean gcp_test

#################################################
# stand alone OP parser

op:
	$(MAKE) -C op

op_prof:
	$(MAKE) -C op prof

op_test:
	$(MAKE) -C op test

op_clean:
	$(MAKE) -C op clean

.PHONY: op op_prof op_clean op_test

#################################################
# stand alone TW parser

tw:
	$(MAKE) -C tw

tw_prof:
	$(MAKE) -C tw prof

tw_test:
	$(MAKE) -C tw test

tw_clean:
	$(MAKE) -C tw clean

.PHONY: tw tw_prof tw_clean tw_test

#################################################
# acceptance

jq:
	jq -e . ./expected/*/*.jsonl > /dev/null

acc_test: jq
	$(MAKE) -C aws acc_test
	$(MAKE) -C gcp acc_test
	$(MAKE) -C op acc_test
	$(MAKE) -C tw acc_test
	@ echo "done"

.PHONY: test jq

#################################################
# valgrind

vg : vg_cmn vg_op vg_gcp vg_aws vg_tw

vg_cmn:
	$(MAKE) -C common vg

vg_aws :
	$(MAKE) -C aws vg

vg_gcp :
	$(MAKE) -C gcp vg

vg_op :
	$(MAKE) -C op vg

vg_tw :
	$(MAKE) -C tw vg

.PHONY: vg_cmn vg_op vg_gcp vg_aws vg_tw vg

#-------------------------------------------------------------------------------
# fuzz testing

clean_fuzz:
	$(MAKE) -C aws clean_fuzz
	$(MAKE) -C gcp clean_fuzz
	$(MAKE) -C op clean_fuzz
	$(MAKE) -C tw clean_fuzz

run_fuzz:
	$(MAKE) -C aws fuzz
	$(MAKE) -C gcp fuzz
	$(MAKE) -C op fuzz
	$(MAKE) -C tw fuzz

.phony: clean_fuzz run_fuzz

