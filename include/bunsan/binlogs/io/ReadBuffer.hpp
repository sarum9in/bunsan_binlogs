#pragma once

#include "bunsan/binlogs/io/BaseBuffer.hpp"

#include <google/protobuf/io/zero_copy_stream.h>

namespace bunsan {
namespace binlogs {
namespace io {

class ReadBuffer: public virtual BaseBuffer {
public:
    virtual google::protobuf::io::ZeroCopyInputStream *istream()=0;
};

}
}
}
