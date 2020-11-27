#!/usr/bin/env bash

protoc --python_out=. ./experiment.proto
python -m grpc_tools.protoc --python_out=. -I=. --grpc_python_out=. experiment.proto
