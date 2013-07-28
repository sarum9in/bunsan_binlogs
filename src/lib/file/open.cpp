#include "bunsan/binlogs/file/open.hpp"
#include "bunsan/binlogs/file/ReadBuffer.hpp"
#include "bunsan/binlogs/file/WriteBuffer.hpp"

#include "bunsan/binlogs/detail/make_unique.hpp"

#include <boost/assert.hpp>

namespace bunsan {
namespace binlogs {
namespace file {

std::unique_ptr<binlogs::ReadBuffer> openReadOnly(
    const boost::filesystem::path &path,
    std::string *error)
{
    std::unique_ptr<ReadBuffer> buffer_ = detail::make_unique<ReadBuffer>();
    if (!buffer_->open(path)) {
        BOOST_VERIFY(buffer_->error(error));
        buffer_.reset();
    }
    return std::move(buffer_);
}

std::unique_ptr<binlogs::WriteBuffer> openWriteOnly(
    const boost::filesystem::path &path,
    std::string *error)
{
    std::unique_ptr<WriteBuffer> buffer_ = detail::make_unique<WriteBuffer>();
    if (!buffer_->open(path)) {
        BOOST_VERIFY(buffer_->error(error));
        buffer_.reset();
    }
    return std::move(buffer_);
}

}
}
}

