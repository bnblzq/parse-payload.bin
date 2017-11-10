//
// Created by liu on 2017/11/3.
//

#include "extra_info.h"
#include "fstream"
#include "iostream"
#include "constant.h"
#include <stdio.h>
#include <string.h>
#include <bitset>
using namespace std;


#define TEST_AND_RETURN(_x) \
    do{ \
        bool _success = static_cast<bool>(_x);\
        if(!_success) { \
            cout<<"line: "<<__LINE__ <<" "<<#_x<<" is not set"<<endl; \
return; \
} \
}while(0)

#define PARTING_LINE(_x) \
do{ \
for(int i=0;i< static_cast<int>(_x);++i) cout<<"-";\
cout<<endl;}\
 while(0)



static const char * TransformToString( InstallOperation_Type type) ;
static void printOpsType(const InstallOperation & op);
//------------------------------------------

bool InstallInfo::ReadDataInBytes(string file ,int size) {
    if(vec_.size() != 0) vec_.clear();

    //content_.open(PAYLOAD_FILE, std::ios::binary);
    content_.open(file, std::ios::binary);
    if(content_){
        content_.unsetf(std::ios::skipws);
        vec_.resize(size);
        content_.seekg(0,std::ios::beg);
        content_.read(&vec_[0], size);
        content_.close();
    }else{
        cout<<file<<" doesnt exist"<<endl;
        return false;
    }
    return true;

}

void InstallInfo::GetBasicHeaderInfo() {

    if(memcmp(&vec_[0], kDeltaMagic, sizeof(kDeltaMagic)) != 0){
        cout<<"bad file format"<<endl;
        return;
    }else cout <<"begin magic : CrAU"<<endl;


    memcpy(&major_payload_version_,&vec_[kDeltaVersionOffset],kDeltaVersionSize );
    major_payload_version_ = be64toh(major_payload_version_);
    if(major_payload_version_ == kBrilloMajorPayloadVersion ){
        cout<<"version:"<<major_payload_version_<<endl;
    }else{
        cout<<"we don't supported"<<endl;
        return ;
    }

    manifest_offset_ = kDeltaManifestSizeOffset + kDeltaManifestSizeSize + kDeltaMetadataSignatureSizeSize;
    cout<<"manifest_offset:"<<manifest_offset_<<endl;
    memcpy(&manifest_size_,&vec_[kDeltaManifestSizeOffset],kDeltaManifestSizeSize);
    manifest_size_ = be64toh(manifest_size_);
    cout<<"manifest_size_:"<<manifest_size_<<endl;

    memcpy(&metadata_signature_size_, &vec_[metadata_signature_size_offset], kDeltaMetadataSignatureSizeSize);
    metadata_signature_size_ = be32toh(metadata_signature_size_);
    cout<<"metadata_signature_size_:"<<metadata_signature_size_<<endl;

    metadata_size_ = manifest_offset_ + manifest_size_;
    cout<<"metadata_size_:"<<metadata_size_<<endl;

}

void InstallInfo::ParseManifest() {

    manifest_.ParseFromArray(&vec_.data()[manifest_offset_], manifest_size_);
    Partition_t part_info;


    for(const PartitionUpdate & partion : manifest_.partitions()){
        partitions_.push_back(partion);
        num_total_operations_ += partion.operations_size();
        acc_num_operations_.push_back(num_total_operations_);

        cout<<partion.partition_name()
            << " has "<<partion.operations_size()<<" operations"<<endl;
    }
    cout<<"num_total_operations_ : "<< num_total_operations_<<endl;

    for (const PartitionUpdate &partition : manifest_.partitions()) {

        part_info.name = partition.partition_name();
        part_info.run_postinstall = partition.has_run_postinstall() &&
                                    partition.run_postinstall();

        if(part_info.run_postinstall){
            part_info.postinstall_path = (partition.has_postinstall_path()?partition.postinstall_path()
                                                                          :kPostinstallDefaultScript);
            part_info.filesystem_type = partition.filesystem_type();
            part_info.postinstall_optional = partition.postinstall_optional();

        }


        if (partition.has_old_partition_info()) {  //incremental use
            const PartitionInfo& info = partition.old_partition_info();
            part_info.source_size = info.size();
            part_info.source_hash.assign(info.hash().begin(), info.hash().end());
        }

        const PartitionInfo& info = partition.new_partition_info();
        part_info.target_size = info.size();
        part_info.target_hash.assign(info.hash().begin(), info.hash().end());

        partition_info.push_back(part_info);

    }
    isManifestParse = true;
}

void InstallInfo::ListPostInfo() {

    TEST_AND_RETURN(isManifestParse);

    PARTING_LINE(20);

    for(const auto& part_info: partition_info) {
        cout << part_info.name << " : " << endl;

        if (!part_info.source_path.empty()) cout << "source_path:" << part_info.source_path << endl;
        if (part_info.source_size != 0) cout << "source_size:" << part_info.source_path << endl;
        if (!part_info.source_hash.empty()) cout << "source_hash:";
        for(const auto  a : part_info.source_hash){
            printf("%02X",(unsigned char)a);
        }

        cout<<endl;

        if (!part_info.target_path.empty()) cout << "target_path:" << part_info.target_path << endl;
        if (part_info.target_size != 0) cout << "target_size:" << part_info.target_size << endl;
        if (!part_info.target_hash.empty()) cout << "target_hash:";

        for(const auto  a : part_info.target_hash){
            printf("%02X",(unsigned char)a);
        }
        cout<<endl;

        if (part_info.run_postinstall) {
            cout << "run_postinstall : true" << endl;
            cout << "postinstall_path : " << part_info.postinstall_path << endl;
            cout << "filesystem_type : " << part_info.filesystem_type << endl;
            cout << "postinstall_optional : " << part_info.postinstall_optional << endl;

        } else cout << "run_postinstall : false" << endl;
       PARTING_LINE(30);
    }
}

void InstallInfo::ListOpsType(bool print) {

    TEST_AND_RETURN(print);

    TEST_AND_RETURN(isManifestParse);

    PARTING_LINE(30);

    while (next_operation_num_ < num_total_operations_) {
        while (next_operation_num_ >= acc_num_operations_[current_partiton_]) {
            current_partiton_++;// it means we turn to next partition's scope
        }

        const size_t partition_operation_num = next_operation_num_ -
                                               (current_partiton_ ? acc_num_operations_[current_partiton_ - 1] : 0);

        const InstallOperation &op = partitions_[current_partiton_].operations(partition_operation_num);
        printOpsType(op);
        next_operation_num_++;
    }
}


static const char * TransformToString( InstallOperation_Type type) {
    switch (type){
        case InstallOperation::REPLACE:
            return "REPLACE";

        case InstallOperation::REPLACE_BZ:
            return "REPLACE_BZ";

        case InstallOperation::REPLACE_XZ:
            return "REPLACE_XZ";

        case InstallOperation::ZERO:
            return  "ZERO";
        case InstallOperation::DISCARD:
            return "DISCARD";

        case InstallOperation::MOVE:
            return "MOVE";

        case InstallOperation::BSDIFF:
            return "BSDIFF";
        case InstallOperation::SOURCE_COPY:
            return "SOURCE_COPY";

        case InstallOperation::SOURCE_BSDIFF:
            return "SOURCE_BSDIFF";

        case InstallOperation::IMGDIFF:
            return "IMGDIFF";

        default:
            return "undefined oparation";
    }
}

static void printOpsType(const InstallOperation & op){
    static int OpsCount{1};

    switch (op.type()){
        case InstallOperation::REPLACE:
            cout<<OpsCount<<":"<<TransformToString(InstallOperation::REPLACE)<<endl;
            OpsCount++;
            break;
        case InstallOperation::REPLACE_BZ:
            cout<<OpsCount<<":"<<TransformToString(InstallOperation::REPLACE_BZ)<<endl;
            OpsCount++;
            break;
        case InstallOperation::REPLACE_XZ:
            cout<<OpsCount<<":"<<TransformToString(InstallOperation::REPLACE_XZ)<<endl;
            OpsCount++;
            break;
        case InstallOperation::ZERO:
            cout<<OpsCount<<":"<<TransformToString(InstallOperation::ZERO)<<endl;
            OpsCount++;
            break;
        case InstallOperation::DISCARD:
            cout<<OpsCount<<":"<<TransformToString(InstallOperation::DISCARD)<<endl;
            OpsCount++;
            break;
        case InstallOperation::MOVE:
            cout<<OpsCount<<":"<<TransformToString(InstallOperation::MOVE)<<endl;
            OpsCount++;
            break;
        case InstallOperation::BSDIFF:
            cout<<OpsCount<<":"<<TransformToString(InstallOperation::BSDIFF)<<endl;
            OpsCount++;
            break;
        case InstallOperation::SOURCE_COPY:
            cout<<OpsCount<<":"<<TransformToString(InstallOperation::SOURCE_COPY)<<endl;
            OpsCount++;
            break;
        case InstallOperation::SOURCE_BSDIFF:
            cout<<OpsCount<<":"<<TransformToString(InstallOperation::SOURCE_BSDIFF)<<endl;
            OpsCount++;
            break;
        case InstallOperation::IMGDIFF:
            cout<<OpsCount<<":"<<TransformToString(InstallOperation::IMGDIFF)<<endl;
            OpsCount++;
            break;
        default:
            cout<<"undefined operation"<<endl;
            break;
    }
}
