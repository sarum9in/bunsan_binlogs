#pragma once

#include <boost/filesystem/path.hpp>

namespace bunsan {
namespace binlogs {
namespace detail {
namespace directory_log {

boost::filesystem::path nextPath(const boost::filesystem::path &path);

}
}
}
}
