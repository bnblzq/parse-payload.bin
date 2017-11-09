//
// Created by liu on 2017/11/3.
//

#ifndef TEST_EXTRA_INFO_H
#define TEST_EXTRA_INFO_H

#include <vector>
#include <fstream>
#include "update_metadata.pb.h"
using namespace chromeos_update_engine;



class InstallInfo{
public:
    bool ReadDataInBytes(int size);
    void GetBasicHeaderInfo();
    void ParseManifest();
    void ListOpsType(bool print);

private:
    DeltaArchiveManifest manifest_;

    std::vector<char> vec_{0};

    std::ifstream content_;

    //basic header info
    uint64_t major_payload_version_{0};
    uint64_t manifest_offset_{0};
    uint64_t manifest_size_{0};
    uint32_t metadata_signature_size_{0};
    uint64_t metadata_size_{0};


    //how many partition in bin
    std::vector<PartitionUpdate> partitions_;

    //operation will be exec
    std::vector<size_t> acc_num_operations_;
    size_t num_total_operations_{0};
    size_t next_operation_num_{0};
    size_t current_partiton_{0};

};



#endif //TEST_EXTRA_INFO_H
