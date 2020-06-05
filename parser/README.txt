prerequisits for building/testing the 3 parser-classes:

$sudo apt-get update
$sudo apt-get upgrade
$sudo apt-get install git build-essential flex bison cmake uuid-dev

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
$git checkout VDB-4214

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
