#include "bunsan/binlogs/io/file/ReadBuffer.hpp"

#include "openFd.hpp"

namespace bunsan {
namespace binlogs {
namespace io {
namespace file {

std::unique_ptr<google::protobuf::io::FileInputStream> ReadBuffer::openFd(
    const boost::filesystem::path &path,
    int &errno_)
{
    return file::openFd<google::protobuf::io::FileInputStream>(path, O_RDONLY, errno_);
}

}
}
}
}
