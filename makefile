.PHONY: ALL

#LOCAL_CPP_INCLUDE :=-I /usr/local/include/google/protobuf/ -I /usr/local/include/gflags/
LOCAL_CPP_INCLUDE :=-I ./protobuf/ -I ./gflags/
#LOCAL_CPP_LIBRARY :=-L /usr/local/lib/
LOCAL_CPP_LIBRARY :=-L ./lib/
LOCAL_LINK := -l protobuf -l gflags -lpthread
LOCAL_CPP_FLAG := -std=c++11

ALL:
	rm -rf a.out
	g++ ${LOCAL_CPP_INCLUDE} ${LOCAL_CPP_LIBRARY} ${LOCAL_CPP_FLAG} main.cpp extra_info.cpp update_metadata.pb.cc ${LOCAL_LINK}

