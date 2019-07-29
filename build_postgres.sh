#!/bin/bash

. $HOME/strides/strides_env.sh

rm -rf "$PGVER" "$PGVER.tar.bz2" "$PGVER.rel.tar.gz"
curl -O "https://ftp.postgresql.org/pub/source/v11.4/$PGVER.tar.bz2"
tar -xaf "$PGVER.tar.bz2"
rm -f "$PGVER.tar.bz2"
cd $PGVER || exit
./configure "--prefix=$PWD"
nice -n 19 make -j 32
make check && make install
cd ..
tar -caf "$PGVER.rel.tar.gz" "$PGVER/bin" "$PGVER/share" "$PGVER/lib"
rm -rf "$PGVER"

