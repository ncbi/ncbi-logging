#!/bin/bash

# shellcheck source=/home/vartanianmh/strides/strides_env.sh
. "$HOME/strides/strides_env.sh"

rm -rf "$PGVER" "$PGVER.tar.bz2" "$PGVER.rel.tar.gz"
curl -O "https://ftp.postgresql.org/pub/source/v12.17/$PGVER.tar.bz2"
tar -xaf "$PGVER.tar.bz2"
rm -f "$PGVER.tar.bz2"
cd "$PGVER" || exit
# iebdev11 is ivybridge, intprod is skylake
export CFLAGS="-march=native -mtune=skylake -mavx -O3"
./configure "--prefix=$PWD"
make -j 32
make check && make install
cd ..
tar -caf "$PGVER.rel.tar.gz" "$PGVER/bin" "$PGVER/share" "$PGVER/lib"
rm -rf "$PGVER"
