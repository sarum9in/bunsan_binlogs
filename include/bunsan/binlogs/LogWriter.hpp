#pragma once

#include "bunsan/binlogs/BaseWriter.hpp"
#include "bunsan/binlogs/MessageTypePool.hpp"
#include "bunsan/binlogs/MessageType.hpp"

#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/message.h>

namespace bunsan {
namespace binlogs {

class LogWriter: public BaseWriter, public MessageTypePool {
public:
    /// \warning Should always be called before object usage.
    virtual bool writeHeader(
        const google::protobuf::FileDescriptorSet &fileDescriptorSet,
        std::string *error=nullptr)=0;

    virtual bool write(const google::protobuf::Message &message, std::string *error=nullptr);

    virtual bool write(const std::string &typeName,
                       const google::protobuf::Message &message,
                       std::string *error=nullptr)=0;
};

}
}
