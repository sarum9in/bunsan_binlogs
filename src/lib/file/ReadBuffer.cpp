#include "bunsan/binlogs/file/ReadBuffer.hpp"

#include "openFd.hpp"

namespace bunsan {
namespace binlogs {
namespace file {

google::protobuf::io::ZeroCopyInputStream *ReadBuffer::istream()
{
    return stream();
}

std::unique_ptr<google::protobuf::io::FileInputStream> ReadBuffer::openFd(
    const boost::filesystem::path &path,
    int &errno_)
{
    return file::openFd<google::protobuf::io::FileInputStream>(path, O_RDONLY, errno_);
}

}
}
}
