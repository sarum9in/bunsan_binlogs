#include "bunsan/binlogs/v1/LogWriter.hpp"
#include "bunsan/binlogs/detail/make_unique.hpp"

#include <google/protobuf/io/coded_stream.h>

#include <boost/assert.hpp>
#include <boost/format.hpp>

namespace bunsan {
namespace binlogs {
namespace v1 {

LogWriter::LogWriter(google::protobuf::io::ZeroCopyOutputStream *const output):
    output_(output)
{
    BOOST_ASSERT(output_);
}

bool LogWriter::writeHeader(const Header &header, std::string *error)
{
    if (!pool_.Init(header, error)) {
        return false;
    }
    headerData_ = detail::make_unique<HeaderData>();
    *headerData_->mutable_proto() = header.proto;
    for (const std::string &type: header.types) {
        headerData_->add_types()->set_name(type);
    }
    if (!write_(nullptr, *headerData_, error)) {
        state_ = State::kBad;
        output_ = nullptr;
        if (error) {
            *error = "Unable to write header: " + *error;
        }
        return false;
    }
    return true;
}

bool LogWriter::write(const std::string &typeName,
                      const google::protobuf::Message &message,
                      std::string *error)
{
    if (output_) {
        BOOST_ASSERT(headerData_);
        // FIXME We should check that message has type defined in header.
        // Or should we create template-based wrapper for this class?
        return write_(&typeName, message, error);
    } else {
        return false;
    }
}

bool LogWriter::write_(const std::string *const typeName,
                       const google::protobuf::Message &message,
                       std::string *error)
{
    if (output_) {
        google::protobuf::io::CodedOutputStream output(output_);
        if (typeName) {
            const google::protobuf::uint32 messageType = pool_.typeId(*typeName);
            if (messageType == MessageTypePool::npos) {
                state_ = State::kFail;
                if (error) {
                    *error = str(boost::format("Type \"%1%\" is not registered."));
                }
                return false;
            }
            output.WriteLittleEndian32(messageType);
            if (hadError(output, "Unable to write message type.", error)) {
                return false;
            }
        }
        const google::protobuf::uint32 messageSize = message.ByteSize();
        output.WriteLittleEndian32(messageSize);
        if (hadError(output, "Unable to write message size.", error)) {
            return false;
        }
        message.SerializeWithCachedSizes(&output);
        if (hadError(output, "Unable to write message.", error)) {
            return false;
        }
        BOOST_ASSERT(!output.HadError());
        state_ = State::kOk;
        return true;
    } else {
        return false;
    }
}

bool LogWriter::hadError(const google::protobuf::io::CodedOutputStream &output,
                         const std::string &msg,
                         std::string *error)
{
    if (output.HadError()) {
        state_ = State::kBad;
        output_ = nullptr;
        if (error) {
            *error = msg;
        }
        return true;
    } else {
        return false;
    }
}

bool LogWriter::close(std::string */*error*/)
{
    state_ = State::kEof;
    output_ = nullptr;
    return true;
}

LogWriter::State LogWriter::state() const
{
    return state_;
}

const binlogs::MessageTypePool &LogWriter::messageTypePool() const
{
    return pool_;
}

}
}
}
