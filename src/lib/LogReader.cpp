#include <bunsan/binlogs/LogReader.hpp>

namespace bunsan {
namespace binlogs {

bool LogReader::read(const std::unique_ptr<google::protobuf::Message> &message)
{
    return read(*message);
}

std::unique_ptr<google::protobuf::Message> LogReader::read()
{
    if (const MessageType *const type = nextMessageType()) {
        auto message = type->newMessage();
        if (read(message)) {
            return message;
        }
    }
    return nullptr;
}

}
}
