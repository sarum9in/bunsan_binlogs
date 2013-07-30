#include "bunsan/binlogs/io/WriteBuffer.hpp"

namespace bunsan {
namespace binlogs {
namespace io {

bool WriteBuffer::Next(void **data, int *size)
{
    return stream()->Next(data, size);
}

void WriteBuffer::BackUp(int count)
{
    stream()->BackUp(count);
}

google::protobuf::int64 WriteBuffer::ByteCount() const
{
    return stream()->ByteCount();
}

}
}
}
