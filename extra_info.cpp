//
// Created by liu on 2017/11/3.
//

#include "extra_info.h"
#include "fstream"
#include "iostream"
#include "constant.h"
#include <stdio.h>
#include <string.h>
using namespace std;

#define  PAYLOAD_FILE "payload.bin"

const char * TransformToString( InstallOperation_Type type) ;
void printOpsType(const InstallOperation & op);
//------------------------------------------

bool InstallInfo::ReadDataInBytes(int size) {
    if(vec_.size() != 0) vec_.clear();

    content_.open(PAYLOAD_FILE, std::ios::binary);
    if(content_){
        content_.unsetf(std::ios::skipws);
        vec_.resize(size);
        content_.seekg(0,std::ios::beg);
        content_.read(&vec_[0], size);
        content_.close();
    }else{
        cout<<PAYLOAD_FILE<<" doesnt exist"<<endl;
        return false;
    }
    return true;

}

void InstallInfo::GetBasicHeaderInfo() {

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

    manifest_.ParseFromArray( &vec_.data()[manifest_offset_],manifest_size_ );

    for(const PartitionUpdate & partion : manifest_.partitions()){
        partitions_.push_back(partion);
        cout<<"found: "<<partion.partition_name()<<endl;
    }

    for(const auto& part: partitions_){
        num_total_operations_ += part.operations_size();
        acc_num_operations_.push_back(num_total_operations_);
        cout<<part.partition_name() <<" has "<<part.operations_size()<<" oprations"<<endl;
    }

}

void InstallInfo::ListOpsType(bool print) {

    while (next_operation_num_ < num_total_operations_) {
        while (next_operation_num_ >= acc_num_operations_[current_partiton_]) {
            current_partiton_++;// it means we turn to next partition's scope
        }

        const size_t partition_operation_num = next_operation_num_ -
                                               (current_partiton_ ? acc_num_operations_[current_partiton_-1] : 0);

        const InstallOperation & op = partitions_[current_partiton_].operations(partition_operation_num);
        if(print) printOpsType(op);
        next_operation_num_++;
    }
}


const char * TransformToString( InstallOperation_Type type) {
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

void printOpsType(const InstallOperation & op){
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
