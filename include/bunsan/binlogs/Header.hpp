#pragma once

#include <bunsan/binlogs/detail/equals.hpp>

#include <google/protobuf/descriptor.pb.h>

#include <string>
#include <vector>

namespace bunsan {
namespace binlogs {

struct Header {
    google::protobuf::FileDescriptorSet proto;
    std::vector<std::string> types;
};

inline bool operator==(const Header &a, const Header &b)
{
    return detail::equals(a.proto, b.proto) && a.types == b.types;
}

}
}
