#pragma once

#include <boost/noncopyable.hpp>

#include <google/protobuf/message.h>

#include <memory>
#include <string>

namespace bunsan {
namespace binlogs {

class MessageType: private boost::noncopyable {
public:
    virtual ~MessageType();

    virtual std::string typeName() const=0;

    virtual std::unique_ptr<google::protobuf::Message> newMessage() const;
    virtual const google::protobuf::Message *prototype() const=0;
};

}
}
