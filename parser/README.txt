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
$cd ..

$$git clone https://github.com/ncbi/ncbi-logging
$cd ncbi-logging
$git checkout VDB-4214
$make
$make runtests
$make json
