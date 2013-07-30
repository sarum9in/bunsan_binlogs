#pragma once

#include "bunsan/binlogs/io/ReadBuffer.hpp"
#include "bunsan/binlogs/io/WriteBuffer.hpp"

#include <boost/filesystem/path.hpp>

#include <memory>

namespace bunsan {
namespace binlogs {
namespace io {
namespace file {

std::unique_ptr<io::ReadBuffer> openReadOnly(
    const boost::filesystem::path &path,
    std::string *error=nullptr);

std::unique_ptr<io::WriteBuffer> openWriteOnly(
    const boost::filesystem::path &path,
    std::string *error=nullptr);

}
}
}
}
