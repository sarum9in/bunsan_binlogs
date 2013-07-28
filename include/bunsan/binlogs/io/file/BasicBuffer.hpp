#pragma once

#include "bunsan/binlogs/io/WriteBuffer.hpp"

#include <google/protobuf/io/zero_copy_stream_impl.h>

#include <boost/filesystem/path.hpp>

#include <system_error>

namespace bunsan {
namespace binlogs {
namespace io {
namespace file {

template <typename BaseBuffer, typename Stream>
class BasicBuffer: public BaseBuffer {
public:
    /// \return false if !closed()
    bool open(const boost::filesystem::path &path)
    {
        if (!closed()) {
            return false;
        }
        stream_ = openFd(path, errno_);
        return !getErrno();
    }

    Stream *stream()
    {
        return stream_.get();
    }

    bool close() override
    {
        if (stream_) {
            stream_->SetCloseOnDelete(false);
            if (!stream_->Close()) {
                errno_ = stream_->GetErrno();
            }
            stream_.reset();
        }
        return !errno_;
    }

    bool closed() const override
    {
        return !stream_;
    }

    bool error(std::string *error=nullptr) const
    {
        if (getErrno()) {
            if (error) {
                *error = std::system_category().message(getErrno());
            }
            return true;
        }
        return false;
    }

    int getErrno() const
    {
        if (stream_) {
            return stream_->GetErrno();
        }
        return errno_;
    }

protected:
    virtual std::unique_ptr<Stream> openFd(const boost::filesystem::path &path, int &errno_)=0;

private:
    std::unique_ptr<Stream> stream_;
    int errno_ = 0;
};

}
}
}
}
