#pragma once

#include <bunsan/binlogs/io/BaseBuffer.hpp>

#include <bunsan/binlogs/detail/make_unique.hpp>

#include <boost/assert.hpp>

namespace bunsan {
namespace binlogs {
namespace io {
namespace filter {

template <typename BaseBuffer, typename StreamImpl>
class BasicBuffer: public BaseBuffer {
public:
    /// \return false if !closed()
    bool open(std::unique_ptr<BaseBuffer> &&source)
    {
        if (!closed()) {
            return false;
        }
        BOOST_ASSERT(source);
        BOOST_ASSERT(!source->closed());
        source_ = std::move(source);
        BOOST_ASSERT(source_);
        stream_ = detail::make_unique<StreamImpl>(this->source());
        return !streamError();
    }

    bool close() override
    {
        const bool streamOk = streamClose();
        stream_.reset();
        const bool sourceOk = source_->close();
        return streamOk && sourceOk;
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
