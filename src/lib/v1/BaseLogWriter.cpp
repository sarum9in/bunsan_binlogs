#include "bunsan/binlogs/v1/BaseLogWriter.hpp"
#include "bunsan/binlogs/detail/make_unique.hpp"

#include <google/protobuf/io/coded_stream.h>

#include <boost/assert.hpp>
#include <boost/format.hpp>

namespace bunsan {
namespace binlogs {
namespace v1 {

const binlogs::MessageTypePool &BaseLogWriter::messageTypePool() const
{
    return messageTypePool__();
}

BaseLogWriter::State BaseLogWriter::write(
    const std::string *const typeName,
    const google::protobuf::Message &message,
    std::string *error)
{
    BOOST_ASSERT(output__());
    State state = State::kOk;
    google::protobuf::io::CodedOutputStream output(output__());
    if (typeName) {
        const google::protobuf::uint32 messageType = messageTypePool__().typeId(*typeName);
        if (messageType == MessageTypePool::npos) {
            state = State::kFail;
            if (error) {
                *error = str(boost::format("Type \"%1%\" is not registered."));
            }
            return state;
        }
        output.WriteLittleEndian32(messageType);
        if (hadError(output, "Unable to write message type.", state, error)) {
            return state;
        }
    }
    const google::protobuf::uint32 messageSize = message.ByteSize();
    output.WriteLittleEndian32(messageSize);
    if (hadError(output, "Unable to write message size.", state, error)) {
        return state;
    }
    message.SerializeWithCachedSizes(&output);
    if (hadError(output, "Unable to write message.", state, error)) {
        return state;
    }
    BOOST_ASSERT(!output.HadError());
    return state;
}

bool BaseLogWriter::hadError(
    const google::protobuf::io::CodedOutputStream &output,
    const std::string &msg,
    State &state,
    std::string *error)
{
    if (output.HadError()) {
        state = State::kBad;
        if (error) {
            *error = msg;
        }
        return true;
    } else {
        return false;
    }
}

}
}
}
