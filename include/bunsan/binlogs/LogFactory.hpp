#pragma once

#include "bunsan/binlogs/LogReader.hpp"
#include "bunsan/binlogs/LogWriter.hpp"

#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/io/zero_copy_stream.h>

#include <memory>

namespace bunsan {
namespace binlogs {

/// \note calls Init()
std::unique_ptr<LogReader> openReadOnly(
    google::protobuf::io::ZeroCopyInputStream *const input,
    std::string *error=nullptr);

/// \note calls Init()
std::unique_ptr<LogWriter> openWriteOnly(
    google::protobuf::io::ZeroCopyOutputStream *const output,
    const Header &header,
    std::string *error=nullptr);

}
}
