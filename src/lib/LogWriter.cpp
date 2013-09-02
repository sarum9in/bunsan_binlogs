#include <bunsan/binlogs/LogWriter.hpp>

namespace bunsan {
namespace binlogs {

void LogWriter::write(const google::protobuf::Message &message)
{
    write(message.GetTypeName(), message);
}

}
}
