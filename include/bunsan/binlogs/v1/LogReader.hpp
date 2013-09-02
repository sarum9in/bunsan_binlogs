#pragma once

#include <bunsan/binlogs/io/ReadBuffer.hpp>
#include <bunsan/binlogs/v1/HeaderData.pb.h>
#include <bunsan/binlogs/v1/MessageTypePool.hpp>

#include <bunsan/binlogs/LogReader.hpp>

#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/message.h>

#include <boost/optional.hpp>

#include <memory>

namespace bunsan {
namespace binlogs {
namespace v1 {

class LogReader: public binlogs::LogReader {
public:
    explicit LogReader(std::unique_ptr<io::ReadBuffer> &&input);

    bool read(google::protobuf::Message &message) override;

    const MessageType *nextMessageType() override;

    void close() override;

    State state() const override;

    const binlogs::MessageTypePool &messageTypePool() const override;

    /*!
     * \brief Check stream's format without parsing messages.
     * Consumes entire stream.
     *
     * \warning For internal library usage only.
     */
    void fastCheck();

private:
    void read_(google::protobuf::Message *message);
    void read_(google::protobuf::Message &message);
    void read_(google::protobuf::uint32 &uint32);

    std::unique_ptr<io::ReadBuffer> input_;
    boost::optional<const MessageType *> nextMessageType_;
    std::unique_ptr<HeaderData> header_;
    std::unique_ptr<MessageTypePool> pool_;
    State state_ = State::kOk;
};

}
}
}
