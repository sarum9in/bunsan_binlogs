#pragma once

#include "bunsan/binlogs/LogReader.hpp"
#include "bunsan/binlogs/LogWriter.hpp"

#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/io/coded_stream.h>

#include <memory>

namespace bunsan {
namespace binlogs {

/// \note calls readHeader()
std::unique_ptr<LogReader> openReadOnly(
    std::unique_ptr<google::protobuf::io::CodedInputStream> &&input,
    std::string *error=nullptr);

/// \note calls writeHeader()
std::unique_ptr<LogWriter> openWriteOnly(
    std::unique_ptr<google::protobuf::io::CodedOutputStream> &&output,
    const google::protobuf::FileDescriptorSet &fileDescriptorSet,
    std::string *error=nullptr);

}
}
