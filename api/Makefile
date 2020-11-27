# this makefile does not build anything, it is here to run tests

test : logmon.proto
	python3 test_api_file_log.py
	python3 test_file_watcher.py

#depends on bq having the right auth: 'python3 test_api_http_log.py'

phony: test

# create ./logmon_pb2.py
pb: logmon.proto
	python -m grpc_tools.protoc --python_out=. -I=. --grpc_python_out=. logmon.proto

phony: pb