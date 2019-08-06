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
    ./s3tojson > s3test.result 2>&1

gunzip -d -c /tmp/mike_logs/gs_prod/20190731/sra-pub-src-1_usage_2019_07_31_22_00_00_00929704ade74e4bb8_v0.gz | head -n 10000 | \
    ./nginxtojson > nginxtest.result 2>&1

if ! diff nginxtest.result nginxtest.expected
then
    echo "nginx diff failed"
fi

if ! diff s3test.result s3test.expected
then
    echo "s3 diff failed"
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

