#include "bunsan/binlogs/v1/LogReader.hpp"
#include "bunsan/binlogs/detail/make_unique.hpp"

#include <boost/assert.hpp>
#include <boost/format.hpp>

namespace bunsan {
namespace binlogs {
namespace v1 {

LogReader::LogReader(std::unique_ptr<google::protobuf::io::CodedInputStream> &&input):
    input_(std::move(input))
{
    BOOST_ASSERT(input_);
}

bool LogReader::readHeader(std::string *error)
{
    header_ = detail::make_unique<HeaderData>();
    if (!read_(*header_, error)) {
        state_ = State::kBad;
        input_.reset();
        if (error) {
            *error = "Unable to parse header: " + *error;
        }
        return false;
    }
    for (const google::protobuf::FileDescriptorProto &proto: header_->proto().file()) {
        if (!db_.Add(proto)) {
            if (error) {
                *error = "File already in db.";
            }
            return false;
        }
    }
    return true;
}

bool LogReader::read(google::protobuf::Message &message, std::string *error)
{
    if (input_) {
        BOOST_ASSERT(header_);
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
                return nullptr;
            }
            nextMessageType_ = typeId;
            // TODO validate *nextMessageType_
        }
        // TODO return message type for typeId
        // return *nextMessageType_;
    } else {
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

        google::protobuf::uint32 messageSize;
        if (!read_(messageSize, "size", error)) {
            return false;
        }
        const google::protobuf::io::CodedInputStream::Limit limit =
            input_->PushLimit(static_cast<int>(messageSize));
        if (!message.ParseFromCodedStream(input_.get())) {
            // FIXME should be kFail and we should skip all message's data
            // TODO do not forget PopLimit(limit)
            state_ = State::kBad;
            input_.reset();
            *error = "Unable to read message.";
            return false;
        }
        if (message.ByteSize() != static_cast<int>(messageSize)) {
            BOOST_ASSERT(message.ByteSize() < static_cast<int>(messageSize));
            state_ = State::kBad;
            input_.reset();
            *error = str(boost::format("Corrupted message: unable to read %1% bytes.") % messageSize);
            return false;
        }
        input_->PopLimit(limit);
        state_ = State::kOk;
        return true;
    } else {
        return false;
    }
}

bool LogReader::read_(google::protobuf::uint32 &uint32, const std::string &field, std::string *error)
{
    if (!input_->ReadLittleEndian32(&uint32)) {
        state_ = State::kBad;
        input_.reset();
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
    input_.reset();
    return true;
}

LogReader::State LogReader::state() const
{
    return state_;
}

}
}
}
