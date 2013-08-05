#pragma once

#include <bunsan/binlogs/io/WriteBuffer.hpp>
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

class BaseLogWriter: public virtual binlogs::LogWriter {
public:
    const binlogs::MessageTypePool &messageTypePool() const override;

protected:
    void setOutput(std::unique_ptr<io::WriteBuffer> &&output);
    bool closeOutput(std::string *error=nullptr);
    bool hasOutput() const;

    bool writeHeader(std::string *error=nullptr);

    State write(const std::string *const typeName,
                const google::protobuf::Message &message,
                std::string *error);

    virtual const v1::MessageTypePool &messageTypePool__() const=0;

private:
    bool hadError(const google::protobuf::io::CodedOutputStream &output,
                  const std::string &msg,
                  State &state,
                  std::string *error);

    std::unique_ptr<io::WriteBuffer> output_;
};

}
}
}
