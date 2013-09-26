#pragma once

#include <bunsan/binlogs/directory_log/Error.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/optional.hpp>

namespace bunsan {
namespace binlogs {
namespace directory_log {
namespace detail {

/// All log files in directory in lexicographical order.
std::vector<boost::filesystem::path> listDir(const boost::filesystem::path &path);

boost::optional<boost::filesystem::path> lastPath(const boost::filesystem::path &path);

boost::filesystem::path nextPath(const boost::filesystem::path &path);

}
}
}
}
