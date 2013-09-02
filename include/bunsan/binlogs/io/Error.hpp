#pragma once

#include <bunsan/binlogs/Error.hpp>

namespace bunsan {
namespace binlogs {
namespace io {

struct Error: virtual binlogs::Error {};

struct OpenError: virtual Error {};
struct CloseError: virtual Error {};
struct OpenedError: virtual Error {};
struct ClosedError: virtual Error {};

}
}
}
