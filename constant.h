//
// Created by liu on 2017/11/3.
//

#ifndef PROTOBUF_CONSTANT_H
#define PROTOBUF_CONSTANT_H


const char kDeltaMagic[4] = {'C', 'r', 'A', 'U'};
const uint64_t kDeltaVersionOffset = sizeof(kDeltaMagic);
const uint64_t kDeltaVersionSize = 8;
const uint64_t kChromeOSMajorPayloadVersion = 1;
const uint64_t kBrilloMajorPayloadVersion = 2;
const uint64_t kDeltaManifestSizeOffset = kDeltaVersionOffset + kDeltaVersionSize;
const uint64_t kDeltaManifestSizeSize = 8;
const uint64_t kDeltaMetadataSignatureSizeSize = 4;
const uint64_t metadata_signature_size_offset = kDeltaManifestSizeOffset + kDeltaManifestSizeSize;




#endif //PROTOBUF_CONSTANT_H
