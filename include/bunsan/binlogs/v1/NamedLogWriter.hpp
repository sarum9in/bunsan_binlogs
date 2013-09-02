#pragma once

#include <bunsan/binlogs/NamedLogWriter.hpp>
#include <bunsan/binlogs/io/WriteBuffer.hpp>
#include <bunsan/binlogs/v1/BaseLogWriter.hpp>
#include <bunsan/binlogs/v1/HeaderData.pb.h>
#include <bunsan/binlogs/v1/MessageTypePool.hpp>

#include <functional>

namespace bunsan {
namespace binlogs {
namespace v1 {

class NamedLogWriter:
    public BaseLogWriter,
    public virtual binlogs::NamedLogWriter {
public:
    typedef std::function<
        std::unique_ptr<io::WriteBuffer> (
            const boost::filesystem::path &, bool /*append*/)> FileOpener;

public:
    NamedLogWriter(const FileOpener &openFile, const Header &header);

    void write(const std::string &typeName,
               const google::protobuf::Message &message) override;

    void close() override;

    State state() const override;

    boost::filesystem::path path() const override;

    void open(const boost::filesystem::path &path) override;

    void append(const boost::filesystem::path &path) override;

    void reopen() override;

    void reopen(const boost::filesystem::path &newPath) override;

    void rotate(const boost::filesystem::path &renameTo) override;

protected:
    const v1::MessageTypePool &messageTypePool__() const override;

    using BaseLogWriter::write;

private:
    void open_(const boost::filesystem::path &path, const bool append);

private:
    FileOpener openFile_;
    v1::MessageTypePool pool_;
    std::unique_ptr<HeaderData> headerData_;
    boost::filesystem::path path_;
    State state_ = State::kEof;
};

}
}
}
