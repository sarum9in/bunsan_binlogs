#pragma once

#include "bunsan/binlogs/v1/HeaderData.pb.h"
#include "bunsan/binlogs/v1/MessageTypePool.hpp"

#include "bunsan/binlogs/LogWriter.hpp"

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/message.h>

#include <memory>

namespace bunsan {
namespace binlogs {
namespace v1 {

class LogWriter: public binlogs::LogWriter {
public:
    explicit LogWriter(std::unique_ptr<google::protobuf::io::CodedOutputStream> &&output);

    bool writeHeader(const Header &header, std::string *error=nullptr) override;

    bool write(const std::string &typeName,
               const google::protobuf::Message &message,
               std::string *error=nullptr) override;

    bool close(std::string *error=nullptr) override;

    State state() const override;

    const binlogs::MessageTypePool &messageTypePool() const override;

private:
    bool write_(const std::string *const typeName,
                const google::protobuf::Message &message,
                std::string *error);

    bool hadError(const std::string &msg, std::string *error);

    std::unique_ptr<google::protobuf::io::CodedOutputStream> output_;
    std::unique_ptr<HeaderData> headerData_;
    MessageTypePool pool_;
    State state_ = State::kOk;
};

}
}
}