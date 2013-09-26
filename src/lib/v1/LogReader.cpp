#include <bunsan/binlogs/v1/LogReader.hpp>

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

#define BUNSAN_BINLOGS_THROW_FROM_INPUT(ERROR, ...) \
    BOOST_SCOPE_EXIT_ALL(this) \
    { \
        state_ = State::kBad; \
        input_ = nullptr; \
    }; \
    try { \
        BOOST_ASSERT(input_); \
        input_->checkError(); \
    } catch (std::exception &) { \
        BOOST_THROW_EXCEPTION(ERROR << enable_nested_current() __VA_ARGS__); \
    } \
    BOOST_THROW_EXCEPTION(ERROR __VA_ARGS__)

LogReader::LogReader(std::unique_ptr<io::ReadBuffer> &&input):
    input_(std::move(input))
{
    BOOST_ASSERT(input_);
    header_ = detail::make_unique<HeaderData>();
    try {
        read_(*header_);
    } catch (std::exception &) {
        state_ = State::kBad;
        input_ = nullptr;
        BOOST_THROW_EXCEPTION(UnableToParseHeaderError() <<
                              enable_nested_current());
    }
    Header header;
    header.proto = header_->proto();
    header.types.resize(header_->types_size());
    for (std::size_t i = 0; i < header.types.size(); ++i) {
        header.types[i] = header_->types(i).name();
    }
    pool_ = detail::make_unique<v1::MessageTypePool>(header);
}

bool LogReader::read(google::protobuf::Message &message)
{
    if (input_) {
        BOOST_ASSERT(header_);
        if (!nextMessageType()) {
            return false;
        }
        read_(message);
        return true;
    } else {
        return false;
    }
}

const MessageType *LogReader::nextMessageType()
{
    if (input_) {
        BOOST_ASSERT(header_);
        if (!nextMessageType_) {
            google::protobuf::uint32 typeId;
            do {
                try {
                    read_(typeId);
                } catch (std::exception &) {
                    BOOST_THROW_EXCEPTION(UnableToReadMessageTypeError() <<
                                          enable_nested_current());
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
                        BUNSAN_BINLOGS_THROW_FROM_INPUT(UnableToReadFooterError());
                    }
                    if (footer != MAGIC_FOOTER) {
                        BUNSAN_BINLOGS_THROW_FROM_INPUT(InvalidFooterError());
                    }
                    if (!retContinue || continue_ != MAGIC_CONTINUE) {
                        state_ = State::kEof;
                        input_ = nullptr;
                        return nullptr;
                    }
                }
            } while (typeId == std::numeric_limits<google::protobuf::uint32>::max());
            nextMessageType_ = pool_->type(typeId);
            if (!*nextMessageType_) {
                state_ = State::kBad;
                input_ = nullptr;
                BOOST_THROW_EXCEPTION(UnknownMessageTypeIdError() <<
                                      UnknownMessageTypeIdError::MessageTypeId(typeId));
            }
        }
        return *nextMessageType_;
    } else {
        return nullptr;
    }
}

void LogReader::read_(google::protobuf::Message *message)
{
    BOOST_ASSERT(input_);
    nextMessageType_ = boost::none;

    google::protobuf::uint32 messageSize;
    try {
        read_(messageSize);
    } catch (std::exception &) {
        BOOST_THROW_EXCEPTION(UnableToReadMessageSizeError() <<
                              enable_nested_current());
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
                BUNSAN_BINLOGS_THROW_FROM_INPUT(UnableToSkipUnreadBytesError());
            }
            state_ = State::kFail;
            BOOST_THROW_EXCEPTION(UnableToReadMessageError());
        }
        if (message->ByteSize() != static_cast<int>(messageSize)) {
            BOOST_ASSERT(message->ByteSize() < static_cast<int>(messageSize));
            BUNSAN_BINLOGS_THROW_FROM_INPUT(MessageReadError(), <<
                                            MessageReadError::MessageSize(messageSize));
        }
    } else {
        if (!input_->Skip(messageSize)) {
            BUNSAN_BINLOGS_THROW_FROM_INPUT(UnableToSkipMessageError());
        }
    }
    state_ = State::kOk;
}

void LogReader::read_(google::protobuf::Message &message)
{
    read_(&message);
}

void LogReader::read_(google::protobuf::uint32 &uint32)
{
    google::protobuf::io::CodedInputStream input(input_.get());
    if (!input.ReadLittleEndian32(&uint32)) {
        BUNSAN_BINLOGS_THROW_FROM_INPUT(ReadLittleEndian32Error());
    }
}

void LogReader::close()
{
    if (input_) {
        BOOST_SCOPE_EXIT_ALL(this)
        {
            state_ = State::kEof;
            input_.reset();
        };
        input_->close();
    }
}

LogReader::State LogReader::state() const
{
    return state_;
}

const binlogs::MessageTypePool &LogReader::messageTypePool() const
{
    return *pool_;
}

void LogReader::fastCheck()
{
    if (input_) {
        BOOST_ASSERT(header_);
        while (nextMessageType()) {
            read_(nullptr);
        }
    }
}

}
}
}
