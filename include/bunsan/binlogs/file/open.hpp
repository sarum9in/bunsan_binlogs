#pragma once

#include "bunsan/binlogs/ReadBuffer.hpp"
#include "bunsan/binlogs/WriteBuffer.hpp"

#include <boost/filesystem/path.hpp>

#include <memory>

namespace bunsan {
namespace binlogs {
namespace file {

std::unique_ptr<binlogs::ReadBuffer> openReadOnly(
    const boost::filesystem::path &path,
    std::string *error=nullptr);

std::unique_ptr<binlogs::WriteBuffer> openWriteOnly(
    const boost::filesystem::path &path,
    std::string *error=nullptr);

}
}
}
