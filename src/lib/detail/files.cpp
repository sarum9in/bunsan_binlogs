#include <bunsan/binlogs/detail/files.hpp>

#include <bunsan/binlogs/io/file/open.hpp>
#include <bunsan/binlogs/io/filter/gzip.hpp>

namespace bunsan {
namespace binlogs {
namespace detail {

std::unique_ptr<io::WriteBuffer> openFileAppendOnly(const boost::filesystem::path &path)
{
    return io::filter::gzip::open(io::file::openAppendOnly(path));
}

std::unique_ptr<io::ReadBuffer> openFileReadOnly(const boost::filesystem::path &path)
{
    return io::filter::gzip::open(io::file::openReadOnly(path));
}

std::unique_ptr<io::WriteBuffer> openFileWriteOnly(const boost::filesystem::path &path)
{
    return io::filter::gzip::open(io::file::openWriteOnly(path));
}

}
}
}
