#pragma once

#include <bunsan/binlogs/Error.hpp>

namespace bunsan {
namespace binlogs {
namespace directory_log {

struct LogWriterError: virtual NamedLogWriterError {};
struct LogWriterFormatError: virtual LogWriterError {};
struct TooManyLogFilesError: virtual LogWriterFormatError {};

}
}
}
