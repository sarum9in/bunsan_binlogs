#pragma once

#include <bunsan/binlogs/Error.hpp>

#include <boost/filesystem/path.hpp>

namespace bunsan {
namespace binlogs {
namespace detail {
namespace directory_log {

struct TooManyLogFilesError: virtual DirectoryLogWriterError {};

/// All log files in directory in lexicographical order.
std::vector<boost::filesystem::path> listDir(const boost::filesystem::path &path);

boost::filesystem::path nextPath(const boost::filesystem::path &path);

}
}
}
}
