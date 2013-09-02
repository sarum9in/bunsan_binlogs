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
    ~BaseLogWriter() override;

    const binlogs::MessageTypePool &messageTypePool() const override;

protected:
    void setOutput(std::unique_ptr<io::WriteBuffer> &&output);
    void closeOutput();
    bool hasOutput() const;

    void writeHeader();
    void writeFooter();
    void writeContinue();

    void write(const std::string *const typeName,
               const google::protobuf::Message &message);

    virtual const v1::MessageTypePool &messageTypePool__() const=0;

private:
    std::unique_ptr<io::WriteBuffer> output_;
};

}
}
}
