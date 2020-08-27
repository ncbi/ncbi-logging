prerequisits for building/testing the 3 parser-classes:

$sudo apt-get update
$sudo apt-get upgrade
$sudo apt-get install git build-essential flex bison cmake uuid-dev jq

$cd ~/devel
$git clone https://github.com/google/googletest
$cd googletest
$cmake .
$make
$sudo make install
$cd ..

$git clone https://github.com/ncbi/ncbi-vdb3
$cd ncbi-vdb3
$git checkout engineering
$cd jwt-tool
$make
$cd ../..

$$git clone https://github.com/ncbi/ncbi-logging
$cd ncbi-logging/parser
$git checkout LOGMON-65
!!! this is likely to change to engineering or master (LOGMON)

-------------------------------------------------
--- for the older ( aka 1 tool for all formats )
-------------------------------------------------
$make
# there are 2 versions build: log2jsn-dbg ( debug-version ) and log2jsn-rel ( release-version )
# the release-version is about 2.5 times faster
# the 2 binaries are in the bin-subdirectory

$make runtests

# Command line options:

# to process an on-premises log, 1 line per log entry (default)
$ cat data/some_events.txt | bin/log2jsn-rel op

# to process an aws log (default), pretty-printed 
$ cat data/aws_bucket_log.log | bin/log2jsn-rel aws readable

# to process a gcp log (default)
$ cat data/gcp_bucket_log.log | bin/log2jsn-rel gcp

-------------------------------------------------
--- for the new ( aka 1 tool for each format )
-------------------------------------------------
$make v2

#the run the tests ( depends on google-test! )
$make test_v2

#how to run the new tools:
#there are 4 of them now: aws2jsn-rel, gcp2jsn-rel, op2jsn-rel and tw2jsn-rel
#they are again in the bin-subdir of the repository

zcat XXX.gz | aws2jsn-rel basename
or
zcat XXX.gz | gcp2jsn-rel basename
or
zcat XXX.gz | op2jsn-rel basename
or
zcat XXX.gz | tw2jsn-rel basename

#each tool produces it's output now into multiple files:
#basename.good.jsonl    ... this is the file you want to use for loading into your database
#basename.review.jsonl  ... this file contains questionable log-lines, for instance wrong ip-format,url...
#basename.bad.jsonl     ... this file contains log-lines recognized as unusable
#basename.unrecog.jsonl ... this file contains log-lines, where the parser failed
#basename.stats.jsonl   ... this file has the final line-counts

#the stats file contains the line-counts for each of the categories above
#example:
{"_total":9,"bad":0,"good":9,"ignored":0,"review":0,"unrecog":0}
#_total = bad + good + ignored + review + unrecog

#the ignored lines are GCP's header-lines.
#the ignored lines are not written into a file.
#the gcp2jsn compares the headerlines against a hardcoded default.
#if the line matches we discard it and count it as ignored
#if it does not match, we write it out and count it as 'review'

#the tools can be run with the -f option ( fast )
#without this option the json-output is created with the json-library from vdb3
#with this option the json-output is created in a hardcoded way without the json-library ( about 3x the speed on a single thread )

#the tools can run single-threaded or multi-threaded
#the default is single-threaded
#but we highly recommend to use the "-t n" option where n is the number of threads
#we run tests with 10..13 threads on large-memory-machines, resulting in a speed up of 6 - 8 times relative to version 1

# the fastest way of runnig the tools is the combination of '-f' and '-t n'.