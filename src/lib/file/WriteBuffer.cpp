#include "bunsan/binlogs/file/WriteBuffer.hpp"

#include "openFd.hpp"

namespace bunsan {
namespace binlogs {
namespace file {

bool WriteBuffer::open(const boost::filesystem::path &path)
{
    if (!closed()) {
        return false;
    }
    istream_ = openFd<google::protobuf::io::FileOutputStream>(path, O_WRONLY, errno_);
    return !getErrno();
}

google::protobuf::io::ZeroCopyOutputStream *WriteBuffer::ostream()
{
    return istream_.get();
}

bool WriteBuffer::close()
{
    if (istream_) {
        if (!istream_->Close()) {
            errno_ = istream_->GetErrno();
        }
        istream_.reset();
    }
    return errno_;
}

bool WriteBuffer::closed() const
{
    return !istream_;
}

int WriteBuffer::getErrno() const
{
    if (istream_) {
        return istream_->GetErrno();
    }
    return errno_;
}

}
}
}
