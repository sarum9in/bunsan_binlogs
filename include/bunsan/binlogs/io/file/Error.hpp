#pragma once

#include <bunsan/binlogs/io/Error.hpp>

namespace bunsan {
namespace binlogs {
namespace io {
namespace file {

struct Error: virtual io::Error {};

struct OpenError: virtual io::OpenError, virtual Error {};
struct CloseError: virtual io::CloseError, virtual Error {};
struct OpenedError: virtual io::OpenedError, virtual Error {};
struct ClosedError: virtual io::ClosedError, virtual Error {};

}
}
}
}
