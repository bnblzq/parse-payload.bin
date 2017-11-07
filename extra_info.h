//
// Created by liu on 2017/11/3.
//

#ifndef TEST_EXTRA_INFO_H
#define TEST_EXTRA_INFO_H

#include <vector>
#include <fstream>
#include "update_metadata.pb.h"
using namespace chromeos_update_engine;


class ExtraInfo{

public:

    bool ReadInfo(int size);

    char GetChar(int index);

    long long GetVecLength();
    std::vector<char> & getVector();

    DeltaArchiveManifest & GetDeltaManifest();

//    uint64_t getManifistSize(void) ;
//    uint64_t getManifistOffset(void) ;
    void setNumOps();
    void ParseManifist();
    void ParseInfo( );
    void ListOpsType();

    void SetPartitions();
    std::vector<PartitionUpdate> & getPartitions();

private:

    DeltaArchiveManifest manifest;

    std::ifstream myfile_;
    std::vector<char> vec_{0};
    std::vector<PartitionUpdate> partitions_;

    std::vector<size_t> acc_num_operations_;
    size_t num_total_operations_{0};
    size_t current_partiton_{0};
    size_t next_operation_num_{0};

    uint64_t metadata_size_{0};
    uint32_t metadata_signature_size_{0};
    uint64_t major_payload_version_{0};
    uint64_t manifest_offset{0};
    uint64_t manifest_size_{0};


};



















#endif //TEST_EXTRA_INFO_H
