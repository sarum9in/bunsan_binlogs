#pragma once

#include <google/protobuf/descriptor.pb.h>

#include <string>
#include <vector>

namespace bunsan {
namespace binlogs {

struct Header {
    google::protobuf::FileDescriptorSet proto;
    std::vector<std::string> types;
};

}
}
