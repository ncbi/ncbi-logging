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
$git checkout engineering
# use the master branch for production tools, engineering for latest stable code; the ongoing development is in the dev branch and may be broken

-------------------------------------------------
--- for the older ( aka 1 tool for all formats )
-------------------------------------------------
$cd v1
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
--- for the new ( aka 1 tool for each format: AWS, GCP, OP, TW, ERR, CL )
-------------------------------------------------
$make

#the run the tests ( depends on google-test! )
$make test

#how to run the new tools:
# there are several of them now: aws2jsn-rel, gcp2jsn-rel, op2jsn-rel and tw2jsn-rel, err2json-rel and cl2json-rel
# they are again in the bin-subdir of the repository
# in addition, there is Python toold to extract Cloudian data from Splunk (splunk/ directory)

zcat XXX.gz | aws2jsn-rel basename
or
zcat XXX.gz | gcp2jsn-rel basename
or
zcat XXX.gz | op2jsn-rel basename
or
zcat XXX.gz | tw2jsn-rel basename
etc.

#each tool produces it's output now into multiple files:
# basename.good.jsonl    ... this is the file you want to use for loading into your database
# basename.review.jsonl  ... this file contains questionable log-lines, for instance wrong ip-format,url...
# basename.bad.jsonl     ... this file contains log-lines recognized as unusable
# basename.unrecog.jsonl ... this file contains log-lines, where the parser failed
# basename.stats.jsonl   ... this file has the final line-counts

#the stats file contains the line-counts for each of the categories above
#example:
{"_total":9,"bad":0,"good":9,"ignored":0,"review":0,"unrecog":0}
#_total = bad + good + ignored + review + unrecog

#the ignored lines are GCP's header-lines and such
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

# the fastest way of running the tools is the combination of '-f' and '-t n'.

-------------------------------------------------
--- reverse-tool
-------------------------------------------------

the default make-target builds every tool (including the reverse-tool)
the reverse-tools are also in the bin-subdir of the repository
they are named named: jsn2aws-rel jsn2gcp-rel etc.

how to run the reverse tools:

cat XXX.json | jsn2aws-rel basename
etc.

the tool expects its input also via stdin
the tool expects a sequence of json-objects, not a single json-array of objects
the tool expects a json-object per line
empty lines are reported as unrecognized lines
the tool procudes its output into multiple files ( same as the XXX2jsn-tools )
the extension is ".aws" etc, per format
(substitute the format for .aws in the following lines):

basename.good.aws      ... this is the file you want to use for re-applying the aws2jsn-tool on it
basename.review.aws    ... unused, empty
basename.bad.aws     ... unused, empty
basename.unrecog.aws ... empty lines, or lines with invalid json
basename.stats.aws   ... this file has the final line-counts

command line option -f is accepted but ignored for now
command line option -t is accepted (however, since -f has no effect in this tool the performance will be limited by global locks, for now)

the tests for the reverse tool are integrated with the global "test"-target ( depends on google-test! )

-------------------------------------------------
--- Splunk extractor tool
-------------------------------------------------

The sub-directory splunk/ contains a Python tool to extract Cloudian access logs from Splunk via REST API.

cd splunk/
$ ./splunkreq.py -h
usage: splunkreq.py [-h] [--bearer filename] [--earliest time] [--latest time]
                    [--timeout timeout in seconds]

SPLUNK data retrieval tool

optional arguments:
  -h, --help            show this help message and exit
  --bearer filename, -b filename
                        name of the file with bearer token for Splunk access
                        inside
  --earliest time, -e time
                        start time
  --latest time, -l time
                        end time
  --timeout timeout in seconds, -t timeout in seconds
                        maximum wait time for the request, in seconds (default
                        100 seconds)

For the Splunk-supported formats of time values, see
https://docs.splunk.com/Documentation/Splunk/8.1.0/SearchReference/SearchTimeModifiers

Example:

# all records of a specific hour:
$ ./splunkreq.py --bearer bearer.txt --earliest 10/5/2020:20:00:00 --latest 10/5/2020:21:00:00 --timeout 60

# all records for the last 5 minutes (use the shortened form --option=T if T starts with a '-')
$ ./splunkreq.py --bearer bearer.txt --earliest=-5m

The output of the tool goes into stdout, redirect as required; can be piped directly into the CL parser.
The tool outputs the search status (QUEUED, PARSING, etc. until DONE) to stderr.

