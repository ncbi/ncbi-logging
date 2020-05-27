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
$make runtests

$ make log2jsn_ # build ./bin/log2json

# Command line options:

# to process an on-premises log, 1 line per log entry (default)
$ cat data/some_events.txt | bin/log2jsn op

# to process an aws log (default), pretty-printed 
$ cat data/aws_bucket_log.log | bin/log2jsn aws readable

# to process a gcp log (default)
$ cat data/gcp_bucket_log.log | bin/log2jsn gcp

To modify the log2jsn tool, modify this file: log2jsn.cpp

# rebuild log2jsn, amongst other things:
$ make   

