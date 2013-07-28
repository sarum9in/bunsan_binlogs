#pragma once

#include "bunsan/binlogs/io/BaseBuffer.hpp"

#include <google/protobuf/io/zero_copy_stream.h>

namespace bunsan {
namespace binlogs {
namespace io {

class WriteBuffer: public virtual BaseBuffer {
public:
    virtual google::protobuf::io::ZeroCopyOutputStream *ostream()=0;
};

}
}
}
