#pragma once

#include "bunsan/binlogs/io/ReadBuffer.hpp"
#include "bunsan/binlogs/io/WriteBuffer.hpp"

#include <google/protobuf/io/zero_copy_stream.h>

namespace bunsan {
namespace binlogs {
namespace io {
namespace generic {

template <typename BaseBuffer>
struct get_stream {};

template <>
struct get_stream<io::ReadBuffer> {
    typedef google::protobuf::io::ZeroCopyInputStream type;

    static inline type *get(io::ReadBuffer *const buffer)
    {
        return buffer->istream();
    }
};

template <>
struct get_stream<io::WriteBuffer> {
    typedef google::protobuf::io::ZeroCopyOutputStream type;

    static inline type *get(io::WriteBuffer *const buffer)
    {
        return buffer->ostream();
    }
};

}
}
}
}
