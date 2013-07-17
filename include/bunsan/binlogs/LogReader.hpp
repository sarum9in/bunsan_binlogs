#pragma once

#include "bunsan/binlogs/BaseReader.hpp"
#include "bunsan/binlogs/MessageTypePool.hpp"
#include "bunsan/binlogs/MessageType.hpp"

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/message.h>

#include <memory>

namespace bunsan {
namespace binlogs {

class LogReader: public BaseReader {
public:
    /// \warning Should always be called before object usage.
    virtual bool readHeader(std::string *error=nullptr)=0;

    /// Allocate and read message of appropriate type.
    std::unique_ptr<google::protobuf::Message> read(std::string *error=nullptr);

    bool read(const std::unique_ptr<google::protobuf::Message> &message, std::string *error=nullptr);
    virtual bool read(google::protobuf::Message &message, std::string *error=nullptr)=0;

    /// What message will be read next (if available)?
    virtual const MessageType *nextMessageType(std::string *error=nullptr)=0;

    virtual const MessageTypePool &messageTypePool() const=0;
};

}
}
