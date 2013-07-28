#include "bunsan/binlogs/file/ReadBuffer.hpp"

#include "openFd.hpp"

namespace bunsan {
namespace binlogs {
namespace file {

bool ReadBuffer::open(const boost::filesystem::path &path)
{
    if (!closed()) {
        return false;
    }
    istream_ = openFd<google::protobuf::io::FileInputStream>(path, O_RDONLY, errno_);
    return !getErrno();
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
