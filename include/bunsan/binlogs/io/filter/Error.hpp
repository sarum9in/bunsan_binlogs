#pragma once

#include <bunsan/binlogs/io/Error.hpp>

namespace bunsan {
namespace binlogs {
namespace io {
namespace filter {

struct Error: virtual io::Error {};

struct CloseError: virtual Error {
    typedef boost::error_info<struct TagSourceError, boost::exception_ptr> SourceError;
    typedef boost::error_info<struct TagStreamError, boost::exception_ptr> StreamError;
};

}
}
}
}
