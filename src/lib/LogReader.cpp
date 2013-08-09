#include <bunsan/binlogs/LogReader.hpp>

namespace bunsan {
namespace binlogs {

bool LogReader::read(const std::unique_ptr<google::protobuf::Message> &message, std::string *error)
{
    return read(*message, error);
}

std::unique_ptr<google::protobuf::Message> LogReader::read(std::string *error)
{
    if (const MessageType *const type = nextMessageType(error)) {
        auto message = type->newMessage();
        if (read(message, error)) {
            return message;
        }
    }
    return nullptr;
}

}
}
