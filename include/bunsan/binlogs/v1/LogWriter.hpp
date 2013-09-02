#pragma once

#include <bunsan/binlogs/io/WriteBuffer.hpp>
#include <bunsan/binlogs/v1/BaseLogWriter.hpp>
#include <bunsan/binlogs/v1/HeaderData.pb.h>
#include <bunsan/binlogs/v1/MessageTypePool.hpp>

#include <bunsan/binlogs/LogWriter.hpp>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/message.h>

#include <memory>

namespace bunsan {
namespace binlogs {
namespace v1 {

class LogWriter: public BaseLogWriter {
public:
    LogWriter(const Header &header,
              std::unique_ptr<io::WriteBuffer> &&output);

    void write(const std::string &typeName,
               const google::protobuf::Message &message) override;

    void close() override;

    State state() const override;

protected:
    const v1::MessageTypePool &messageTypePool__() const override;

    using BaseLogWriter::write;

private:
    std::unique_ptr<HeaderData> headerData_;
    MessageTypePool pool_;
    State state_ = State::kOk;
};

}
}
}
