#!/bin/bash

rm -f ./s3tojson ./nginxtojson

g++ --std=c++17 -O3 -march=native \
          -Wall -Wextra -Wpedantic \
          -o s3tojson \
          s3tojson.cpp

g++ --std=c++17 -O3 -march=native \
          -Wall -Wextra -Wpedantic \
          -o nginxtojson \
          nginxtojson.cpp
#          -fsanitize=address \

