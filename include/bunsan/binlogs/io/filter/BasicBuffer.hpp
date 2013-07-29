#pragma once

#include "bunsan/binlogs/io/BaseBuffer.hpp"
#include "bunsan/binlogs/io/generic/get_stream.hpp"

#include "bunsan/binlogs/detail/make_unique.hpp"

#include <boost/assert.hpp>

namespace bunsan {
namespace binlogs {
namespace io {
namespace filter {

template <typename BaseBuffer, typename Stream>
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
        stream_ = detail::make_unique<Stream>(
            generic::get_stream<BaseBuffer>::get(this->source()));
        return !streamError();
    }

    BaseBuffer *source() { return source_.get(); }
    const BaseBuffer *source() const { return source_.get(); }

    Stream *stream() { return stream_.get(); }
    const Stream *stream() const { return stream_.get(); }

    bool close() override
    {
        const bool streamOk = streamClose();
        const bool sourceOk = source_->close();
        stream_.reset();
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

private:
    // note: stream_ should be closed before source_
    std::unique_ptr<BaseBuffer> source_; // may not be null
    std::unique_ptr<Stream> stream_; // nullable
};

}
}
}
}
