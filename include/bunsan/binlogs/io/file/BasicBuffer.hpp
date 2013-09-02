#pragma once

#include <bunsan/binlogs/io/file/Error.hpp>
#include <bunsan/binlogs/io/WriteBuffer.hpp>
#include <bunsan/binlogs/SystemError.hpp>

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
    void open(const boost::filesystem::path &path)
    {
        try {
            if (!closed()) {
                BOOST_THROW_EXCEPTION(OpenedError());
            }
            path_ = path;
            stream_ = openFd(path, errno_);
            checkError();
        } catch (std::exception &) {
            BOOST_THROW_EXCEPTION(OpenError().enable_nested_current());
        }
    }

    void close() override
    {
        try {
            if (stream_) {
                stream_->SetCloseOnDelete(false);
                if (!stream_->Close()) {
                    errno_ = stream_->GetErrno();
                }
                stream_.reset();
            }
            checkError();
        } catch (std::exception &) {
            BOOST_THROW_EXCEPTION(CloseError().enable_nested_current());
        }
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

    void checkError() const override
    {
        if (getErrno()) {
            BOOST_THROW_EXCEPTION(SystemError(getErrno()));
        }
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
