#pragma once

#include <bunsan/binlogs/Header.hpp>
#include <bunsan/binlogs/NamedLogWriter.hpp>

#include <memory>

namespace bunsan {
namespace binlogs {
namespace directory_log {

class LogWriter: public virtual NamedLogWriter {
public:
    explicit LogWriter(const Header &header);

    void write(const std::string &typeName,
               const google::protobuf::Message &message) override;

    const binlogs::MessageTypePool &messageTypePool() const override;

    void close() override;

    State state() const override;

    boost::filesystem::path path() const override;

    void open(const boost::filesystem::path &path) override;

    void append(const boost::filesystem::path &path) override;

    void reopen() override;

    void reopen(const boost::filesystem::path &newPath) override;

    void rotate(const boost::filesystem::path &renameTo) override;

private:
    Header header_;
    boost::filesystem::path path_;
    std::unique_ptr<NamedLogWriter> logWriter_;
};

}
}
}
