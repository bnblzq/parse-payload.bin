.PHONY: ALL

LOCAL_CPP_INCLUDE :=-I /usr/local/protobuf/include/ -I /usr/gflags/include/
LOCAL_CPP_LIBRARY :=-L /usr/local/protobuf/lib/ -L /usr/gflags/lib/
LOCAL_LINK := -l protobuf -l gflags -lpthread
LOCAL_CPP_FLAG := -std=c++11

ALL:
	rm -rf a.out
	g++ ${LOCAL_CPP_INCLUDE} ${LOCAL_CPP_LIBRARY} ${LOCAL_CPP_FLAG} main.cpp extra_info.cpp update_metadata.pb.cc ${LOCAL_LINK}

