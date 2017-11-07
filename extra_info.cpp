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


bool ExtraInfo::ReadInfo(int size) {

    if(vec_.size() != 0) vec_.clear();

    myfile_.open(PAYLOAD_FILE, std::ios::binary);
    if(myfile_){
        myfile_.unsetf(std::ios::skipws);
        vec_.resize(size);
        myfile_.seekg(0,std::ios::beg);
        myfile_.read(&vec_[0],size);
        myfile_.close();
    }else{
        cout<<PAYLOAD_FILE<<" doesnt exist"<<endl;
        return false;
    }
    return true;
}

void ExtraInfo::ParseInfo(void){
    memcpy(&major_payload_version_,&vec_[kDeltaVersionOffset],kDeltaVersionSize );
    major_payload_version_ = be64toh(major_payload_version_);
    if(major_payload_version_ == kBrilloMajorPayloadVersion ){
        cout<<"version:"<<major_payload_version_<<endl;
    }else{
        cout<<"we don't supported"<<endl;
        return ;
    }

    manifest_offset = kDeltaManifestSizeOffset + kDeltaManifestSizeSize + kDeltaMetadataSignatureSizeSize;
    cout<<"manifest_offset:"<<manifest_offset<<endl;

    memcpy(&manifest_size_,&vec_[kDeltaManifestSizeOffset],kDeltaManifestSizeSize);
    manifest_size_ = be64toh(manifest_size_);
    cout<<"manifest_size_:"<<manifest_size_<<endl;

    memcpy(&metadata_signature_size_, &vec_[metadata_signature_size_offset], kDeltaMetadataSignatureSizeSize);
    metadata_signature_size_ = be32toh(metadata_signature_size_);
    cout<<"metadata_signature_size_:"<<metadata_signature_size_<<endl;

    metadata_size_ = manifest_offset + manifest_size_;
    cout<<"metadata_size_:"<<metadata_size_<<endl;

}


char ExtraInfo::GetChar(int index) {
    if(index > vec_.size()){ return NULL;}
    return vec_[index];
}

DeltaArchiveManifest & ExtraInfo::GetDeltaManifest() {
    return manifest;
}

long long ExtraInfo::GetVecLength() {
    return vec_.size();
}

vector<char> & ExtraInfo::getVector() {
    return vec_;
}

vector<PartitionUpdate> & ExtraInfo::getPartitions() {
    return partitions_;
}

//uint64_t ExtraInfo::getManifistSize(void)  {
//    return manifest_size_;
//}
//uint64_t ExtraInfo::getManifistOffset(){
//    return manifest_offset;
//}

void ExtraInfo::SetPartitions() {
    for(const PartitionUpdate & partion : manifest.partitions()){
        //cout<<"name:"<<partion.partition_name()<<endl;
        partitions_.push_back(partion);
        cout<<"pushing "<<partion.partition_name()<<endl;
    }
}

void ExtraInfo::setNumOps(){
    for(const auto& part: partitions_){
        num_total_operations_ += part.operations_size();
        acc_num_operations_.push_back(num_total_operations_);
        //cout<<part.partition_name() <<":"<<part.operations_size()<<endl;
    }
        cout<<"acc_num_operations_[0]"<<acc_num_operations_[0]<<endl
        <<"acc_num_operations_[1]"<<acc_num_operations_[1]<<endl;
}

void ExtraInfo::ParseManifist() {
   // manifest.ParseFromArray(&extra.getVector().data()[extra.getManifistOffset()],extra.getManifistSize());
    manifest.ParseFromArray( &vec_.data()[manifest_offset],manifest_size_ );
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

void ExtraInfo::ListOpsType() {
    while(next_operation_num_ < num_total_operations_){
        while(next_operation_num_ >= acc_num_operations_[current_partiton_]){
            current_partiton_ ++;// it means we turn to next partition's scope
        }

        const size_t partition_operation_num = next_operation_num_ -
                (current_partiton_ ? acc_num_operations_[current_partiton_-1] : 0);

        const InstallOperation & op = partitions_[current_partiton_].operations(partition_operation_num);
        printOpsType(op);
        next_operation_num_++;
    }
}