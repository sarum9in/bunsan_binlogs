#pragma once

#include <bunsan/binlogs/Error.hpp>

#include <boost/filesystem/path.hpp>

namespace bunsan {
namespace binlogs {
namespace detail {
namespace directory_log {

struct TooManyLogFilesError: virtual DirectoryLogWriterError {};

boost::filesystem::path nextPath(const boost::filesystem::path &path);

}
}
}
}
