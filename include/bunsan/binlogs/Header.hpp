#pragma once

#include <bunsan/binlogs/detail/equals.hpp>

#include <google/protobuf/descriptor.pb.h>

#include <string>
#include <vector>

namespace bunsan {
namespace binlogs {

/*! \note Keeping this class simple struct
 * allows us to use {.proto = proto, .types = types}
 * initialization.
 */
struct Header {
    google::protobuf::FileDescriptorSet proto;
    std::vector<std::string> types;
};

inline bool operator==(const Header &a, const Header &b)
{
    return detail::equals(a.proto, b.proto) && a.types == b.types;
}

inline bool operator!=(const Header &a, const Header &b)
{
    return !(a == b);
}

}
}
