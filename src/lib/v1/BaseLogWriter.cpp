#include <bunsan/binlogs/v1/BaseLogWriter.hpp>

#include <bunsan/binlogs/detail/make_unique.hpp>
#include <bunsan/binlogs/v1/Error.hpp>
#include <bunsan/binlogs/v1/format.hpp>

#include <google/protobuf/io/coded_stream.h>

#include <boost/assert.hpp>
#include <boost/scope_exit.hpp>

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

void BaseLogWriter::closeOutput()
{
    if (output_) {
        BOOST_SCOPE_EXIT_ALL(this)
        {
            output_.reset();
        };
        try {
            writeFooter();
        } catch (std::exception &) {
            output_.reset();
            BOOST_THROW_EXCEPTION(UnableToCloseOutputError().enable_nested_current());
        }
    }
}

bool BaseLogWriter::hasOutput() const
{
    return static_cast<bool>(output_);
}

#define BUNSAN_BINLOGS_THROW_FROM_OUTPUT(ERROR, ...) \
    try { \
        BOOST_ASSERT(output_); \
        output_->checkError(); \
    } catch (std::exception &) { \
        BOOST_THROW_EXCEPTION(ERROR.enable_nested_current() __VA_ARGS__); \
    } \
    BOOST_THROW_EXCEPTION(ERROR __VA_ARGS__)

void BaseLogWriter::writeFooter()
{
    BOOST_ASSERT(output_);
    google::protobuf::io::CodedOutputStream output(output_.get());
    output.WriteLittleEndian32(std::numeric_limits<google::protobuf::uint32>::max());
    output.WriteRaw(&MAGIC_FOOTER, MAGIC_FOOTER.size());
    if (output.HadError()) {
        BUNSAN_BINLOGS_THROW_FROM_OUTPUT(UnableToWriteFooterError());
    }
}

void BaseLogWriter::writeContinue()
{
    BOOST_ASSERT(output_);
    google::protobuf::io::CodedOutputStream output(output_.get());
    output.WriteRaw(&MAGIC_CONTINUE, MAGIC_CONTINUE.size());
    if (output.HadError()) {
        BUNSAN_BINLOGS_THROW_FROM_OUTPUT(UnableToWriteContinueMagicError());
    }
}

const binlogs::MessageTypePool &BaseLogWriter::messageTypePool() const
{
    return messageTypePool__();
}

#define BUNSAN_BINLOGS_TRY_THROW_FROM_OUTPUT(ERROR, ...) \
    if (output.HadError()) { \
        BUNSAN_BINLOGS_THROW_FROM_OUTPUT(ERROR, __VA_ARGS__); \
    }

void BaseLogWriter::write(
    const std::string *const typeName,
    const google::protobuf::Message &message)
{
    BOOST_ASSERT(output_);
    google::protobuf::io::CodedOutputStream output(output_.get());
    if (!message.IsInitialized()) {
        std::vector<std::string> fields;
        message.FindInitializationErrors(&fields);
        BOOST_THROW_EXCEPTION(UninitializedMessageError() <<
                              Recoverable(true) <<
                              UninitializedMessageError::UninitializedFields(fields) <<
                              UninitializedMessageError::MessageTypeName(message.GetTypeName()));
    }
    if (typeName) {
        const google::protobuf::uint32 messageType = messageTypePool__().typeId(*typeName);
        if (messageType == MessageTypePool::npos) {
            BOOST_THROW_EXCEPTION(UnknownMessageTypeError() <<
                                  Recoverable(true) <<
                                  UnknownMessageTypeError::MessageTypeName(*typeName));
        }
        output.WriteLittleEndian32(messageType);
        BUNSAN_BINLOGS_TRY_THROW_FROM_OUTPUT(UnableToWriteMessageTypeError());
    }
    const google::protobuf::uint32 messageSize = message.ByteSize();
    output.WriteLittleEndian32(messageSize);
    BUNSAN_BINLOGS_TRY_THROW_FROM_OUTPUT(UnableToWriteMessageSizeError());
    message.SerializeWithCachedSizes(&output);
    BUNSAN_BINLOGS_TRY_THROW_FROM_OUTPUT(UnableToWriteMessageError());
    BOOST_ASSERT(!output.HadError());
}

}
}
}
