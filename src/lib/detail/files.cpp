#include <bunsan/binlogs/detail/files.hpp>

#include <bunsan/binlogs/io/file/open.hpp>
#include <bunsan/binlogs/io/filter/gzip.hpp>

namespace bunsan {
namespace binlogs {
namespace detail {

std::unique_ptr<io::ReadBuffer> openFileReadOnly(
    const boost::filesystem::path &path,
    std::string *error)
{
    std::unique_ptr<io::ReadBuffer> buffer = io::file::openReadOnly(path, error);
    if (!buffer) return nullptr;
    return io::filter::gzip::open(std::move(buffer), error);
}

std::unique_ptr<io::WriteBuffer> openFileWriteOnly(
    const boost::filesystem::path &path,
    std::string *error)
{
    std::unique_ptr<io::WriteBuffer> buffer = io::file::openWriteOnly(path, error);
    if (!buffer) return nullptr;
    return io::filter::gzip::open(std::move(buffer), error);
}

}
}
}
