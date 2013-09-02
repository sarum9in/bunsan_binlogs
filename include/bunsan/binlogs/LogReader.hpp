#pragma once

#include <bunsan/binlogs/BaseReader.hpp>
#include <bunsan/binlogs/MessageTypePool.hpp>
#include <bunsan/binlogs/MessageType.hpp>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/message.h>

#include <memory>

namespace bunsan {
namespace binlogs {

class LogReader: public BaseReader {
public:
    /// Allocate and read message of appropriate type.
    std::unique_ptr<google::protobuf::Message> read();

    /// False means EOF.
    bool read(const std::unique_ptr<google::protobuf::Message> &message);

    /// False means EOF.
    virtual bool read(google::protobuf::Message &message)=0;

    /// What message will be read next (if available)?
    virtual const MessageType *nextMessageType()=0;

    virtual const MessageTypePool &messageTypePool() const=0;
};

}
}
