#pragma once

#include "bunsan/binlogs/BaseWriter.hpp"
#include "bunsan/binlogs/Header.hpp"
#include "bunsan/binlogs/MessageTypePool.hpp"
#include "bunsan/binlogs/MessageType.hpp"

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/message.h>

namespace bunsan {
namespace binlogs {

class LogWriter: public BaseWriter {
public:
    /// \warning Should always be called before object usage.
    virtual bool Init(const Header &header, std::string *error=nullptr)=0;

    virtual bool write(const google::protobuf::Message &message, std::string *error=nullptr);

    virtual bool write(const std::string &typeName,
                       const google::protobuf::Message &message,
                       std::string *error=nullptr)=0;

    virtual const MessageTypePool &messageTypePool() const=0;
};

}
}
