#pragma once

#include "bunsan/binlogs/io/BaseBuffer.hpp"

#include <google/protobuf/io/zero_copy_stream.h>

namespace bunsan {
namespace binlogs {
namespace io {

class WriteBuffer:
    public virtual BaseBuffer,
    public virtual google::protobuf::io::ZeroCopyOutputStream {
public:
    typedef google::protobuf::io::ZeroCopyOutputStream Stream;

public:
    bool Next(void **data, int *size) override;
    void BackUp(int count) override;
    google::protobuf::int64 ByteCount() const override;

protected:
    virtual Stream *stream()=0;
    virtual const Stream *stream() const=0;
};

}
}
}
