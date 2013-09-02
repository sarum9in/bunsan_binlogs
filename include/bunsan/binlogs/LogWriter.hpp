#pragma once

#include <bunsan/binlogs/BaseWriter.hpp>
#include <bunsan/binlogs/Header.hpp>
#include <bunsan/binlogs/MessageTypePool.hpp>
#include <bunsan/binlogs/MessageType.hpp>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/message.h>

namespace bunsan {
namespace binlogs {

class LogWriter: public BaseWriter {
public:
    virtual void write(const google::protobuf::Message &message);

    virtual void write(const std::string &typeName,
                       const google::protobuf::Message &message)=0;

    virtual const MessageTypePool &messageTypePool() const=0;
};

}
}
