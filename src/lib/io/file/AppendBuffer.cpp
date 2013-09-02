#include <bunsan/binlogs/io/file/AppendBuffer.hpp>

#include "openFd.hpp"

namespace bunsan {
namespace binlogs {
namespace io {
namespace file {

std::unique_ptr<google::protobuf::io::FileOutputStream> AppendBuffer::openFd(
    const boost::filesystem::path &path,
    int &errno_)
{
    return file::openFd<google::protobuf::io::FileOutputStream>(path, O_APPEND | O_WRONLY, errno_);
}

}
}
}
}
