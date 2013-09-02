#include <bunsan/binlogs/io/file/open.hpp>
#include <bunsan/binlogs/io/file/AppendBuffer.hpp>
#include <bunsan/binlogs/io/file/ReadBuffer.hpp>
#include <bunsan/binlogs/io/file/WriteBuffer.hpp>

#include <bunsan/binlogs/detail/make_unique.hpp>

#include <boost/assert.hpp>

namespace bunsan {
namespace binlogs {
namespace io {
namespace file {

std::unique_ptr<io::WriteBuffer> openAppendOnly(
    const boost::filesystem::path &path)
{
    std::unique_ptr<AppendBuffer> buffer_ = detail::make_unique<AppendBuffer>();
    buffer_->open(path);
    return std::move(buffer_);
}

std::unique_ptr<io::ReadBuffer> openReadOnly(
    const boost::filesystem::path &path)
{
    std::unique_ptr<ReadBuffer> buffer_ = detail::make_unique<ReadBuffer>();
    buffer_->open(path);
    return std::move(buffer_);
}

std::unique_ptr<io::WriteBuffer> openWriteOnly(
    const boost::filesystem::path &path)
{
    std::unique_ptr<WriteBuffer> buffer_ = detail::make_unique<WriteBuffer>();
    buffer_->open(path);
    return std::move(buffer_);
}

}
}
}
}
