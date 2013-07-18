#include "bunsan/binlogs/v1/LogReader.hpp"
#include "bunsan/binlogs/detail/make_unique.hpp"

#include <google/protobuf/io/coded_stream.h>

#include <boost/assert.hpp>
#include <boost/format.hpp>

namespace bunsan {
namespace binlogs {
namespace v1 {

LogReader::LogReader(google::protobuf::io::ZeroCopyInputStream *const input):
    input_(input)
{
    BOOST_ASSERT(input_);
}

bool LogReader::readHeader(std::string *error)
{
    header_ = detail::make_unique<HeaderData>();
    if (!read_(*header_, error)) {
        state_ = State::kBad;
        input_ = nullptr;
        if (error) {
            *error = "Unable to parse header: " + *error;
        }
        return false;
    }
    Header header;
    header.proto = header_->proto();
    header.types.resize(header_->types_size());
    for (std::size_t i = 0; i < header.types.size(); ++i) {
        header.types[i] = header_->types(i).name();
    }
    if (!pool_.Init(header, error)) {
        return false;
    }
    return true;
}

bool LogReader::read(google::protobuf::Message &message, std::string *error)
{
    if (input_) {
        BOOST_ASSERT(header_);
        if (!nextMessageType(error)) {
            return false;
        }
        return read_(message, error);
    } else {
        return false;
    }
}

const MessageType *LogReader::nextMessageType(std::string *error)
{
    if (input_) {
        BOOST_ASSERT(header_);
        if (!nextMessageType_) {
            google::protobuf::uint32 typeId;
            if (!read_(typeId, "type", error)) {
                state_ = State::kEof; // TODO check underlying stream
                return nullptr;
            }
            nextMessageType_ = pool_.type(typeId);
            if (!*nextMessageType_) {
                state_ = State::kBad;
                input_ = nullptr;
                if (error) {
                    *error = str(boost::format("Unknown type id = %1%.") % typeId);
                }
                return nullptr;
            }
        }
        return *nextMessageType_;
    } else {
        if (error) {
            *error = "End of file.";
        }
        return nullptr;
    }
}

bool LogReader::read_(google::protobuf::Message &message, std::string *error)
{
    if (input_) {
        std::string error_;
        if (!error) {
            error = &error_;
        }

        google::protobuf::io::CodedInputStream input(input_);

        google::protobuf::uint32 messageSize;
        if (!read_(messageSize, "size", error)) {
            return false;
        }
        const google::protobuf::io::CodedInputStream::Limit limit =
            input.PushLimit(static_cast<int>(messageSize));
        const bool messageReadResult = message.ParseFromCodedStream(&input);
        input.PopLimit(limit);
        if (!messageReadResult) {
            // note: we ignore return value
            // because Skip() may try to read past limit and it is IO error
            (void) input_->Skip(messageSize);
            state_ = State::kFail;
            *error = "Unable to read message.";
            return false;
        }
        if (message.ByteSize() != static_cast<int>(messageSize)) {
            BOOST_ASSERT(message.ByteSize() < static_cast<int>(messageSize));
            state_ = State::kBad;
            input_ = nullptr;
            *error = str(boost::format("Corrupted message: unable to read %1% bytes.") % messageSize);
            return false;
        }
        nextMessageType_ = boost::none;
        state_ = State::kOk;
        return true;
    } else {
        return false;
    }
}

bool LogReader::read_(google::protobuf::uint32 &uint32, const std::string &field, std::string *error)
{
    google::protobuf::io::CodedInputStream input(input_);
    if (!input.ReadLittleEndian32(&uint32)) {
        state_ = State::kBad;
        input_ = nullptr;
        if (error) {
            *error = str(boost::format("Unable to read message %1%.") % field);
        }
        return false;
    }
    return true;
}

bool LogReader::close(std::string */*error*/)
{
    state_ = State::kEof;
    input_ = nullptr;
    return true;
}

LogReader::State LogReader::state() const
{
    return state_;
}

const binlogs::MessageTypePool &LogReader::messageTypePool() const
{
    return pool_;
}

}
}
}
