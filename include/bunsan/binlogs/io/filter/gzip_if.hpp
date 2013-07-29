#pragma once

#include "bunsan/binlogs/io/ReadBuffer.hpp"
#include "bunsan/binlogs/io/WriteBuffer.hpp"

#include <memory>

namespace bunsan {
namespace binlogs {
namespace io {
namespace filter {
namespace gzip {

/*!
 * \note Uses file magic.
 * \note See RFC 1952
 * \return buffer if not GZIP
 */
std::unique_ptr<ReadBuffer> openIfGzip(std::unique_ptr<ReadBuffer> &&buffer, std::string *error=nullptr);

}
}
}
}
}
