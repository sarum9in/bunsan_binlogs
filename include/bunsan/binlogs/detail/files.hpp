#pragma once

#include <bunsan/binlogs/io/ReadBuffer.hpp>
#include <bunsan/binlogs/io/WriteBuffer.hpp>

#include <boost/filesystem/path.hpp>

#include <memory>

namespace bunsan {
namespace binlogs {
namespace detail {

std::unique_ptr<io::WriteBuffer> openFileAppendOnly(const boost::filesystem::path &path);

std::unique_ptr<io::ReadBuffer> openFileReadOnly(const boost::filesystem::path &path);

std::unique_ptr<io::WriteBuffer> openFileWriteOnly(const boost::filesystem::path &path);

}
}
}
