#pragma once

#include "bunsan/binlogs/io/WriteBuffer.hpp"
#include "bunsan/binlogs/v1/BaseLogWriter.hpp"
#include "bunsan/binlogs/v1/HeaderData.pb.h"
#include "bunsan/binlogs/v1/MessageTypePool.hpp"

#include "bunsan/binlogs/LogWriter.hpp"

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/message.h>

#include <memory>

namespace bunsan {
namespace binlogs {
namespace v1 {

class LogWriter: public BaseLogWriter {
public:
    explicit LogWriter(std::unique_ptr<io::WriteBuffer> &&output);

    bool Init(const Header &header, std::string *error=nullptr) override;

    bool write(const std::string &typeName,
               const google::protobuf::Message &message,
               std::string *error=nullptr) override;

    bool close(std::string *error=nullptr) override;

    State state() const override;

protected:
    io::WriteBuffer *output__() override;

    const v1::MessageTypePool &messageTypePool__() const override;

    using BaseLogWriter::write;

private:
    std::unique_ptr<io::WriteBuffer> output_;
    std::unique_ptr<HeaderData> headerData_;
    MessageTypePool pool_;
    State state_ = State::kOk;
};

}
}
}
