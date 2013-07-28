#include "bunsan/binlogs/file/WriteBuffer.hpp"

#include "openFd.hpp"

namespace bunsan {
namespace binlogs {
namespace file {

google::protobuf::io::ZeroCopyOutputStream *WriteBuffer::ostream()
{
    return stream();
}

std::unique_ptr<google::protobuf::io::FileOutputStream> WriteBuffer::openFd(
    const boost::filesystem::path &path,
    int &errno_)
{
    return file::openFd<google::protobuf::io::FileOutputStream>(path, O_RDONLY, errno_);
}

}
}
}
