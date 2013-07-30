#include "bunsan/binlogs/io/ReadBuffer.hpp"

namespace bunsan {
namespace binlogs {
namespace io {

bool ReadBuffer::Next(const void **data, int *size)
{
    return stream()->Next(data, size);
}

void ReadBuffer::BackUp(int count)
{
    stream()->BackUp(count);
}

bool ReadBuffer::Skip(int count)
{
    return Skip(count);
}

google::protobuf::int64 ReadBuffer::ByteCount() const
{
    return stream()->ByteCount();
}

}
}
}
