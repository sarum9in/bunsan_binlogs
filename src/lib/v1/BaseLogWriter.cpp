#include <bunsan/binlogs/v1/BaseLogWriter.hpp>

#include <bunsan/binlogs/detail/make_unique.hpp>
#include <bunsan/binlogs/v1/format.hpp>

#include <google/protobuf/io/coded_stream.h>

#include <boost/assert.hpp>
#include <boost/format.hpp>

#include <limits>

namespace bunsan {
namespace binlogs {
namespace v1 {

void BaseLogWriter::setOutput(std::unique_ptr<io::WriteBuffer> &&output)
{
    BOOST_ASSERT(!output_);
    output_ = std::move(output);
    BOOST_ASSERT(output_);
}

bool BaseLogWriter::closeOutput(std::string *error)
{
    bool ret = true;
    if (output_) {
        ret = writeFooter(error);
        if (ret) {
            ret = output_->close();
            if (!ret) {
                BOOST_VERIFY(output_->error(error));
            }
        }
        output_.reset();
    }
    return ret;
}

bool BaseLogWriter::hasOutput() const
{
    return static_cast<bool>(output_);
}

bool BaseLogWriter::writeFooter(std::string *error)
{
    BOOST_ASSERT(output_);
    google::protobuf::io::CodedOutputStream output(output_.get());
    output.WriteLittleEndian32(std::numeric_limits<google::protobuf::uint32>::max());
    output.WriteRaw(&MAGIC_FOOTER, MAGIC_FOOTER.size());
    if (hadError(output, "Unable to write footer.", nullptr, error)) {
        return false;
    }
    return true;
}

bool BaseLogWriter::writeContinue(std::string *error)
{
    BOOST_ASSERT(output_);
    google::protobuf::io::CodedOutputStream output(output_.get());
    output.WriteRaw(&MAGIC_CONTINUE, MAGIC_CONTINUE.size());
    if (hadError(output, "Unable to write continue magic.", nullptr, error)) {
        return false;
    }
    return true;
}

const binlogs::MessageTypePool &BaseLogWriter::messageTypePool() const
{
    return messageTypePool__();
}

BaseLogWriter::State BaseLogWriter::write(
    const std::string *const typeName,
    const google::protobuf::Message &message,
    std::string *error)
{
    BOOST_ASSERT(output_);
    State state = State::kOk;
    google::protobuf::io::CodedOutputStream output(output_.get());
    if (!message.IsInitialized()) {
        state = State::kFail;
        if (error) {
            *error = str(boost::format("Unable to write incomplete message of %1% type, "
                                       "uninitialized fields: [%2%].") %
                message.GetTypeName() %
                message.InitializationErrorString());
        }
        return state;
    }
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
        if (hadError(output, "Unable to write message type.", &state, error)) {
            return state;
        }
    }
    const google::protobuf::uint32 messageSize = message.ByteSize();
    output.WriteLittleEndian32(messageSize);
    if (hadError(output, "Unable to write message size.", &state, error)) {
        return state;
    }
    message.SerializeWithCachedSizes(&output);
    if (hadError(output, "Unable to write message.", &state, error)) {
        return state;
    }
    BOOST_ASSERT(!output.HadError());
    return state;
}

bool BaseLogWriter::hadError(
    const google::protobuf::io::CodedOutputStream &output,
    const std::string &msg,
    State *state,
    std::string *error)
{
    if (output.HadError()) {
        if (state) {
            *state = State::kBad;
        }
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
