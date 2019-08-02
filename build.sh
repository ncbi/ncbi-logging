#!/bin/bash

rm -f ./s3tojson ./nginxtojson

g++ --std=c++17 -O3 -march=native \
 -Wall -Wextra -Wpedantic \
 -flto \
 -o s3tojson \
 s3tojson.cpp

g++ --std=c++17 -O3 -march=native \
 -Wall -Wextra -Wpedantic \
 -flto \
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

