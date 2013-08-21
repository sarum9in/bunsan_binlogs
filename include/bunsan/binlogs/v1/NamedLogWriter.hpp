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
            const boost::filesystem::path &, bool /*append*/, std::string *)> FileOpener;

public:
    explicit NamedLogWriter(const FileOpener &openFile);

    bool Init(const Header &header, std::string *error=nullptr) override;

    bool write(const std::string &typeName,
               const google::protobuf::Message &message,
               std::string *error=nullptr) override;

    bool close(std::string *error=nullptr) override;

    State state() const override;

    boost::filesystem::path path() const override;

    bool open(const boost::filesystem::path &path, std::string *error=nullptr) override;

    bool append(const boost::filesystem::path &path, std::string *error=nullptr) override;

    bool reopen(std::string *error=nullptr) override;

    bool reopen(const boost::filesystem::path &newPath, std::string *error=nullptr) override;

    bool rotate(const boost::filesystem::path &renameTo, std::string *error=nullptr) override;

protected:
    const v1::MessageTypePool &messageTypePool__() const override;

    using BaseLogWriter::write;

private:
    bool open_(const boost::filesystem::path &path, const bool append, std::string *error);

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
