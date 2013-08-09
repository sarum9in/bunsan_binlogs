#include <bunsan/binlogs/io/file/open.hpp>
#include <bunsan/binlogs/io/file/ReadBuffer.hpp>
#include <bunsan/binlogs/io/file/WriteBuffer.hpp>

#include <bunsan/binlogs/detail/make_unique.hpp>

#include <boost/assert.hpp>

namespace bunsan {
namespace binlogs {
namespace io {
namespace file {

std::unique_ptr<io::ReadBuffer> openReadOnly(
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

std::unique_ptr<io::WriteBuffer> openWriteOnly(
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
}
