#pragma once

#include "bunsan/binlogs/LogWriter.hpp"
#include "bunsan/binlogs/v1/HeaderData.pb.h"

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/message.h>

#include <memory>

namespace bunsan {
namespace binlogs {
namespace v1 {

class LogWriter: public binlogs::LogWriter {
public:
    explicit LogWriter(std::unique_ptr<google::protobuf::io::CodedOutputStream> &&output);

    bool writeHeader(
        const google::protobuf::FileDescriptorSet &fileDescriptorSet,
        std::string *error=nullptr) override;

    bool write(const google::protobuf::Message &message, std::string *error=nullptr) override;

    bool close(std::string *error=nullptr) override;

    State state() const override;

private:
    bool write_(const google::protobuf::Message &message, std::string *error);

    std::unique_ptr<google::protobuf::io::CodedOutputStream> output_;
    std::unique_ptr<HeaderData> headerData_;
    State state_ = State::kOk;
};

}
}
}
