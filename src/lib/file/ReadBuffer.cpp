#include "bunsan/binlogs/file/ReadBuffer.hpp"

#include "bunsan/binlogs/detail/make_unique.hpp"

#include <boost/scope_exit.hpp>

#include <fcntl.h>

namespace bunsan {
namespace binlogs {
namespace file {

bool ReadBuffer::open(const boost::filesystem::path &path)
{
    if (!closed()) {
        return false;
    }
    errno_ = 0;
    int fd = ::open(path.c_str(), O_RDONLY);
    BOOST_SCOPE_EXIT_ALL(&fd)
    {
        if (fd) {
            ::close(fd);
        }
    };
    if (fd < 0) {
        errno_ = errno;
        return false;
    }
    istream_ = detail::make_unique<google::protobuf::io::FileInputStream>(fd);
    fd = 0; // disable scope exit
    return true;
}

google::protobuf::io::ZeroCopyInputStream *ReadBuffer::istream()
{
    return istream_.get();
}

bool ReadBuffer::close()
{
    if (istream_) {
        if (!istream_->Close()) {
            errno_ = istream_->GetErrno();
        }
        istream_.reset();
    }
    return errno_;
}

bool ReadBuffer::closed() const
{
    return !istream_;
}

int ReadBuffer::getErrno() const
{
    if (istream_) {
        return istream_->GetErrno();
    }
    return errno_;
}

}
}
}
