#pragma once

#include <bunsan/binlogs/io/ReadBuffer.hpp>
#include <bunsan/binlogs/io/WriteBuffer.hpp>
#include <bunsan/binlogs/LogReader.hpp>
#include <bunsan/binlogs/LogWriter.hpp>
#include <bunsan/binlogs/NamedLogWriter.hpp>

#include <boost/filesystem/path.hpp>

#include <memory>

namespace bunsan {
namespace binlogs {

/// \note calls Init()
std::unique_ptr<LogReader> openReadOnly(
    std::unique_ptr<io::ReadBuffer> &&input,
    std::string *error=nullptr);

/// \note calls Init()
std::unique_ptr<LogReader> openReadOnly(
    const boost::filesystem::path &path,
    std::string *error=nullptr);

/// \note calls Init()
std::unique_ptr<LogWriter> openWriteOnly(
    std::unique_ptr<io::WriteBuffer> &&output,
    const Header &header,
    std::string *error=nullptr);

/// \note calls Init()
std::unique_ptr<NamedLogWriter> newWriter(
    const Header &header,
    std::string *error=nullptr);

/// \note calls Init()
std::unique_ptr<NamedLogWriter> openWriteOnly(
    const boost::filesystem::path &path,
    const Header &header,
    std::string *error=nullptr);

/// \note calls Init()
std::unique_ptr<NamedLogWriter> openAppendOnly(
    const boost::filesystem::path &path,
    const Header &header,
    std::string *error=nullptr);

}
}
