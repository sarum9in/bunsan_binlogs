#include <bunsan/binlogs/v1/LogReader.hpp>

#include <bunsan/binlogs/detail/make_unique.hpp>
#include <bunsan/binlogs/v1/format.hpp>

#include <google/protobuf/io/coded_stream.h>

#include <boost/assert.hpp>
#include <boost/format.hpp>

#include <limits>

namespace bunsan {
namespace binlogs {
namespace v1 {

LogReader::LogReader(std::unique_ptr<io::ReadBuffer> &&input):
    input_(std::move(input))
{
    BOOST_ASSERT(input_);
}

bool LogReader::Init(std::string *error)
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
            do {
                if (!read_(typeId, "type", error)) {
                    state_ = State::kBad;
                    input_ = nullptr;
                    return nullptr;
                }
                if (typeId == std::numeric_limits<google::protobuf::uint32>::max()) {
                    bool retFooter, retContinue;
                    boost::uuids::uuid footer, continue_;
                    {
                        google::protobuf::io::CodedInputStream input(input_.get());
                        retFooter = input.ReadRaw(&footer, footer.size());
                        retContinue = input.ReadRaw(&continue_, continue_.size());
                    }
                    if (!retFooter) {
                        state_ = State::kBad;
                        if (error) {
                            if (input_->error(error)) {
                                *error = str(boost::format("Unable to read footer: %1%") % *error);
                            } else {
                                *error = "Unable to read footer.";
                            }
                        }
                        input_ = nullptr;
                        return nullptr;
                    }
                    if (footer != MAGIC_FOOTER) {
                        state_ = State::kBad;
                        input_ = nullptr;
                        if (error) {
                            *error = "Invalid footer.";
                        }
                        return nullptr;
                    }
                    if (!retContinue || continue_ != MAGIC_CONTINUE) {
                        state_ = State::kEof;
                        input_ = nullptr;
                        if (error) {
                            *error = "End of file.";
                        }
                        return nullptr;
                    }
                }
            } while (typeId == std::numeric_limits<google::protobuf::uint32>::max());
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

bool LogReader::read_(google::protobuf::Message *message, std::string *error)
{
    if (input_) {
        nextMessageType_ = boost::none;

        std::string error_;
        if (!error) {
            error = &error_;
        }

        google::protobuf::uint32 messageSize;
        if (!read_(messageSize, "size", error)) {
            state_ = State::kBad;
            return false;
        }

        if (message) {
            bool messageReadResult;
            int bytesLeft = 0;
            {
                google::protobuf::io::CodedInputStream input(input_.get());
                const google::protobuf::io::CodedInputStream::Limit limit =
                    input.PushLimit(static_cast<int>(messageSize));
                messageReadResult = message->ParseFromCodedStream(&input);
                bytesLeft = input.BytesUntilLimit();
                BOOST_ASSERT(bytesLeft >= 0);
                input.PopLimit(limit);
            }
            if (!messageReadResult || bytesLeft != 0) {
                if (!input_->Skip(bytesLeft)) {
                    *error = "Unable to skip unread bytes.";
                    state_ = State::kBad;
                    input_ = nullptr;
                    return false;
                }
                state_ = State::kFail;
                *error = "Unable to read message.";
                return false;
            }
            if (message->ByteSize() != static_cast<int>(messageSize)) {
                BOOST_ASSERT(message->ByteSize() < static_cast<int>(messageSize));
                state_ = State::kBad;
                input_ = nullptr;
                *error = str(boost::format("Corrupted message: unable to read %1% bytes.") % messageSize);
                return false;
            }
        } else {
            if (!input_->Skip(messageSize)) {
                *error = "Unable to skip message.";
                state_ = State::kBad;
                input_ = nullptr;
                return false;
            }
        }
        state_ = State::kOk;
        return true;
    } else {
        return false;
    }
}

bool LogReader::read_(google::protobuf::Message &message, std::string *error)
{
    return read_(&message, error);
}

bool LogReader::read_(google::protobuf::uint32 &uint32, const std::string &field, std::string *error)
{
    google::protobuf::io::CodedInputStream input(input_.get());
    if (!input.ReadLittleEndian32(&uint32)) {
        if (input_->error(error)) {
            state_ = State::kBad;
        } else {
            state_ = State::kEof;
        }
        input_ = nullptr;
        if (error) {
            *error = str(boost::format(
                "Unable to read message %1%: %2%.") % field % *error);
        }
        return false;
    }
    return true;
}

bool LogReader::close(std::string *error)
{
    state_ = State::kEof;
    const bool ret = input_->close();
    if (!ret) {
        BOOST_VERIFY(input_->error(error));
    }
    input_.reset();
    return ret;
}

LogReader::State LogReader::state() const
{
    return state_;
}

const binlogs::MessageTypePool &LogReader::messageTypePool() const
{
    return pool_;
}

bool LogReader::fastCheck(std::string *error)
{
    if (input_) {
        BOOST_ASSERT(header_);
        for (;;) {
            if (!nextMessageType(error)) {
                return state_ == State::kEof;
            }
            if (!read_(nullptr, error)) {
                return false;
            }
        }
    } else {
        if (error) {
            *error = "Stream is closed.";
        }
        return false;
    }
}

}
}
}
