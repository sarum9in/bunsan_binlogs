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

    bool Init(std::string *error=nullptr) override;

    bool read(google::protobuf::Message &message, std::string *error=nullptr) override;

    const MessageType *nextMessageType(std::string *error=nullptr) override;

    bool close(std::string *error=nullptr) override;

    State state() const override;

    const binlogs::MessageTypePool &messageTypePool() const override;

    /*!
     * \brief Check stream's format without parsing messages.
     * Consumes entire stream.
     *
     * \warning For internal library usage only.
     */
    bool fastCheck(std::string *error=nullptr);

private:
    bool read_(google::protobuf::Message *message, std::string *error);
    bool read_(google::protobuf::Message &message, std::string *error);
    bool read_(google::protobuf::uint32 &uint32, const std::string &field, std::string *error);

    std::unique_ptr<io::ReadBuffer> input_;
    boost::optional<const MessageType *> nextMessageType_;
    std::unique_ptr<HeaderData> header_;
    MessageTypePool pool_;
    State state_ = State::kOk;
};

}
}
}
