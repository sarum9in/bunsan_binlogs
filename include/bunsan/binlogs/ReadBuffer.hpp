#pragma once

#include "bunsan/binlogs/BaseBuffer.hpp"

#include <google/protobuf/io/zero_copy_stream.h>

namespace bunsan {
namespace binlogs {

class ReadBuffer: public virtual BaseBuffer {
public:
    virtual google::protobuf::io::ZeroCopyInputStream *istream()=0;
};

}
}
