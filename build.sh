#!/bin/bash

rm -f ./s3tojson ./nginxtojson

g++ --std=c++17 -O3 -march=native \
 -Wall -Wextra -Wpedantic \
 -flto \
 -fprofile-generate \
 -o s3tojson \
 s3tojson.cpp

g++ --std=c++17 -O3 -march=native \
 -Wall -Wextra -Wpedantic \
 -flto \
 -fprofile-generate \
 -o nginxtojson \
 nginxtojson.cpp

gunzip -d -c /tmp/mike_logs/s3_prod/20190731.combine.gz | head -n 10000 | \
    ./s3tojson 2>&1 | sort > s3test.result

gunzip -d -c \
    /tmp/mike_logs/sra_prod/20190731/panfspan1.be-md.ncbi.nlm.nih.govapplog_db_tmpdatabaselogarchiveftp.httplocal_archive20190731srafiles34access.log_20190731.logbuff11.gz \
    | head -n 10000 | \
    ./nginxtojson 2>&1 | sort > nginxtest.result

if ! diff nginxtest.result nginxtest.expected
then
    echo "nginx diff failed"
    exit 1
fi

if ! diff s3test.result s3test.expected
then
    echo "s3 diff failed"
    exit 1
fi

g++ --std=c++17 -O3 -march=native \
 -Wall -Wextra -Wpedantic \
 -flto \
 -fprofile-use \
 -o s3tojson \
 s3tojson.cpp

g++ --std=c++17 -O3 -march=native \
 -Wall -Wextra -Wpedantic \
 -flto \
 -fprofile-use \
 -o nginxtojson \
 nginxtojson.cpp



# -fsanitize=address \
exit 0

clang-tidy -fix \
 -checks='*,-cppcoreguidelines-pro-bounds-pointer-arithmetic,-cppcoreguidelines-pro-type-vararg,-hicpp-vararg,-fuchsia-default-arguments,-cppcoreguidelines-pro-bounds-array-to-pointer-decay,-hicpp-no-array-decay' \
 s3tojson.cpp -- \
 -std=c++17 \ -I. -I/opt/ncbi/gcc/7.3.0/include/c++/7.3.0/ \
 -I/opt/ncbi/gcc/7.3.0/include/c++/7.3.0/x86_64-redhat-linux-gnu/

clang-tidy -fix \
 -checks='*,-cppcoreguidelines-pro-bounds-pointer-arithmetic,-cppcoreguidelines-pro-type-vararg,-hicpp-vararg,-fuchsia-default-arguments,-cppcoreguidelines-pro-bounds-array-to-pointer-decay,-hicpp-no-array-decay' \
 nginxtojson.cpp -- \
 -std=c++17 \ -I. -I/opt/ncbi/gcc/7.3.0/include/c++/7.3.0/ \
 -I/opt/ncbi/gcc/7.3.0/include/c++/7.3.0/x86_64-redhat-linux-gnu/

scan-build --use-analyzer /usr/local/llvm/7.0.0/bin/clang g++ \
 --std=c++17 -O3 -march=native -Wall -Wextra -Wpedantic \
 -I. -I/opt/ncbi/gcc/7.3.0/include/c++/7.3.0/ \
 -I/opt/ncbi/gcc/7.3.0/include/c++/7.3.0/x86_64-redhat-linux-gnu/ \
 nginxtojson.cpp

scan-build --use-analyzer /usr/local/llvm/7.0.0/bin/clang g++ \
 --std=c++17 -O3 -march=native -Wall -Wextra -Wpedantic \
 -I. -I/opt/ncbi/gcc/7.3.0/include/c++/7.3.0/ \
 -I/opt/ncbi/gcc/7.3.0/include/c++/7.3.0/x86_64-redhat-linux-gnu/ \
 s3tojson.cpp

unset ASAN_OPTIONS
export AFL_SKIP_CPUFREQ=1
export AFL_HARDEN=1
export AFL_PATH="$HOME/AFL"
export AFL_NO_AFFINITY=1
CC="$HOME/bin/afl-gcc"
CXX="$HOME/bin/afl-g++"
afl-g++ --std=c++17 -g -march=native -Wall -Wextra -Wpedantic  -o s3tojson s3tojson.cpp
afl-g++ --std=c++17 -g -march=native -Wall -Wextra -Wpedantic  -o nginxtojson nginxtojson.cpp
afl-fuzz -i fuzz_s3 -o findings_s3 -- ./s3tojson
afl-fuzz -i fuzz_nginx -o findings_nginx -- ./nginxtojson

# Parallel fuzzing
afl-fuzz -M fuzzer00    -i fuzz_s3 -o findings_s3 -- ./s3tojson
for x in $(seq 20); do
afl-fuzz -S "fuzzer0$x" -i fuzz_s3 -o findings_s3 -- ./s3tojson > /dev/null 2>&1 &
done
