#pragma once

#include <bunsan/binlogs/detail/make_unique.hpp>
#include <bunsan/binlogs/io/BaseBuffer.hpp>
#include <bunsan/binlogs/io/filter/Error.hpp>

#include <boost/assert.hpp>

namespace bunsan {
namespace binlogs {
namespace io {
namespace filter {

template <typename BaseBuffer, typename StreamImpl>
class BasicBuffer: public BaseBuffer {
public:
    /// \return false if !closed()
    void open(std::unique_ptr<BaseBuffer> &&source)
    {
        try {
            if (!closed()) {
                BOOST_THROW_EXCEPTION(OpenedError());
            }
            BOOST_ASSERT(source);
            BOOST_ASSERT(!source->closed());
            source_ = std::move(source);
            BOOST_ASSERT(source_);
            stream_ = detail::make_unique<StreamImpl>(this->source());
            checkError();
        } catch (std::exception &) {
            BOOST_THROW_EXCEPTION(OpenError() <<
                                  enable_nested_current());
        }
    }

    void close() override
    {
        boost::exception_ptr streamError, sourceError;
        try {
            streamClose();
        } catch (std::exception &) {
            streamError = boost::current_exception();
        }
        stream_.reset();
        try {
            source_->close();
        } catch (std::exception &) {
            sourceError = boost::current_exception();
        }
        if (streamError && sourceError) {
            BOOST_THROW_EXCEPTION(CloseError() <<
                                  CloseError::StreamError(streamError) <<
                                  CloseError::SourceError(sourceError));
        }
        if (streamError) {
            BOOST_THROW_EXCEPTION(CloseError() <<
                                  CloseError::StreamError(streamError));
        }
        if (sourceError) {
            BOOST_THROW_EXCEPTION(CloseError() <<
                                  CloseError::SourceError(sourceError));
        }
    }

    bool closed() const override
    {
        return !stream_;
    }

    bool error(std::string *error=nullptr) const
    {
        if (source_->error(error)) {
            return true;
        }
        if (streamError(error)) {
            return true;
        }
        return false;
    }

    using BaseBuffer::checkError;

protected:
    virtual bool streamError(std::string *error=nullptr) const=0;
    virtual bool streamClose()=0;

    typename BaseBuffer::Stream *stream() override { return stream_.get(); }
    const typename BaseBuffer::Stream *stream() const override { return stream_.get(); }

    BaseBuffer *source() { return source_.get(); }
    const BaseBuffer *source() const { return source_.get(); }

    StreamImpl *stream__() { return stream_.get(); }
    const StreamImpl *stream__() const { return stream_.get(); }

private:
    // note: stream_ should be closed before source_
    std::unique_ptr<BaseBuffer> source_; // may not be null
    std::unique_ptr<StreamImpl> stream_; // nullable
};

}
}
}
}
