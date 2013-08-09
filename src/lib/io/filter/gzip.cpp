#include <bunsan/binlogs/io/filter/gzip.hpp>
#include <bunsan/binlogs/io/filter/BasicBuffer.hpp>

#include <bunsan/binlogs/detail/make_unique.hpp>

#include <google/protobuf/io/gzip_stream.h>

#include <boost/assert.hpp>

namespace bunsan {
namespace binlogs {
namespace io {
namespace filter {
namespace gzip {

namespace {

template <typename BaseBuffer, typename Stream>
class GzipBasicBuffer: public BasicBuffer<BaseBuffer, Stream> {
protected:
    bool streamError(std::string *error=nullptr) const override
    {
        if (streamError_()) {
            if (error) {
                *error = streamError_();
            }
            return true;
        }
        return false;
    }

    bool streamClose() override
    {
        if (this->stream__()->ZlibErrorMessage()) {
            error_ = detail::make_unique<std::string>(this->stream__()->ZlibErrorMessage());
        }
        // Gzip*Stream does not require closing
        return true;
    }

private:
    const char *streamError_() const
    {
        if (this->closed()) {
            if (error_) {
                return error_->c_str();
            }
            return nullptr;
        }
        return this->stream__()->ZlibErrorMessage();
    }

private:
    std::unique_ptr<std::string> error_;
};

class GzipReadBuffer: public GzipBasicBuffer<ReadBuffer, google::protobuf::io::GzipInputStream> {};
class GzipWriteBuffer: public GzipBasicBuffer<WriteBuffer, google::protobuf::io::GzipOutputStream> {};

}

std::unique_ptr<ReadBuffer> open(std::unique_ptr<ReadBuffer> &&buffer, std::string *error)
{
    std::unique_ptr<GzipReadBuffer> gzip = detail::make_unique<GzipReadBuffer>();
    if (!gzip->open(std::move(buffer))) {
        BOOST_VERIFY(gzip->error(error));
        gzip.reset();
    }
    return std::move(gzip);
}

std::unique_ptr<WriteBuffer> open(std::unique_ptr<WriteBuffer> &&buffer, std::string *error)
{
    std::unique_ptr<GzipWriteBuffer> gzip = detail::make_unique<GzipWriteBuffer>();
    if (!gzip->open(std::move(buffer))) {
        BOOST_VERIFY(gzip->error(error));
        gzip.reset();
    }
    return std::move(gzip);
}

}
}
}
}
}
