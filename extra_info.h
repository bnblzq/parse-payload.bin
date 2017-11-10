//
// Created by liu on 2017/11/3.
//

#ifndef TEST_EXTRA_INFO_H
#define TEST_EXTRA_INFO_H

#include <vector>
#include <fstream>
#include "update_metadata.pb.h"

using namespace chromeos_update_engine;

typedef struct Partition {
    bool operator==(const Partition& that) const;

    // The name of the partition.
    std::string name;

    std::string source_path;
    uint64_t source_size{0};
    std::vector<char> source_hash;

    std::string target_path;
    uint64_t target_size{0};
    std::vector<char> target_hash;

    // Whether we should run the postinstall script from this partition and the
    // postinstall parameters.
    bool run_postinstall{false};
    std::string postinstall_path;
    std::string filesystem_type;
    bool postinstall_optional{false};

}Partition_t;


class InstallInfo{
public:
    bool ReadDataInBytes(std::string file,int size);
    void GetBasicHeaderInfo();
    void ParseManifest();
    void ListOpsType(bool print);
    void ListPostInfo();

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


    //how many operations inside each partition
    std::vector<PartitionUpdate> partitions_;

    //addition info inside each partition . maybe this should include above vector ?
    std::vector<Partition_t> partition_info;

    //operation will be exec
    std::vector<size_t> acc_num_operations_;
    size_t num_total_operations_{0};
    size_t next_operation_num_{0};
    size_t current_partiton_{0};


    bool isManifestParse{false};
};



#endif //TEST_EXTRA_INFO_H
