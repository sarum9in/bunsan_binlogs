#include "bunsan/binlogs/LogWriter.hpp"

namespace bunsan {
namespace binlogs {

bool LogWriter::write(const google::protobuf::Message &message, std::string *error)
{
    return write(message.GetTypeName(), message, error);
}

}
}
