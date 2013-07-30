#pragma once

#include "bunsan/binlogs/io/WriteBuffer.hpp"

#include <google/protobuf/io/zero_copy_stream_impl.h>

#include <boost/filesystem/path.hpp>
#include <boost/format.hpp>

#include <system_error>

namespace bunsan {
namespace binlogs {
namespace io {
namespace file {

template <typename BaseBuffer, typename StreamImpl>
class BasicBuffer: public BaseBuffer {
public:
    /// \return false if !closed()
    bool open(const boost::filesystem::path &path)
    {
        if (!closed()) {
            return false;
        }
        path_ = path;
        stream_ = openFd(path, errno_);
        return !getErrno();
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
                *error = str(boost::format("%1%: %2%") %
                    path_ %
                    std::system_category().message(getErrno()));
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
    virtual std::unique_ptr<StreamImpl> openFd(const boost::filesystem::path &path, int &errno_)=0;

    typename BaseBuffer::Stream *stream() override { return stream_.get(); }
    const typename BaseBuffer::Stream *stream() const override { return stream_.get(); }

    StreamImpl *stream__()
    {
        return stream_.get();
    }

private:
    boost::filesystem::path path_;
    std::unique_ptr<StreamImpl> stream_;
    int errno_ = 0;
};

}
}
}
}
