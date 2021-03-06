#pragma once

#include <bunsan/binlogs/LogReader.hpp>

#include <boost/filesystem/path.hpp>

#include <memory>
#include <vector>

namespace bunsan {
namespace binlogs {
namespace directory_log {

// TODO how to open log with invalid first file?
class LogReader: public binlogs::LogReader {
public:
    explicit LogReader(const boost::filesystem::path &path);

    bool read(google::protobuf::Message &message) override;

    const MessageType *nextMessageType() override;

    void close() override;

    State state() const override;

    const binlogs::MessageTypePool &messageTypePool() const override;

    void fastCheck() override;

private:
    /// \return if logReader_ can be accessed
    bool update();

private:
    std::vector<boost::filesystem::path> paths_;
    std::size_t next_;

    // invariant: logReader_ is not null
    std::unique_ptr<binlogs::LogReader> logReader_;
    // last update has failed, logReader_ is outdated
    bool fail_;
};

}
}
}
