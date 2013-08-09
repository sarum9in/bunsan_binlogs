#include <bunsan/binlogs/MessageType.hpp>

namespace bunsan {
namespace binlogs {

MessageType::~MessageType() {}

std::unique_ptr<google::protobuf::Message> MessageType::newMessage() const
{
    std::unique_ptr<google::protobuf::Message> msg(prototype()->New());
    return msg;
}

}
}
