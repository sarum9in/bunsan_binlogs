#include <bunsan/binlogs/directory_log/LogReader.hpp>

#include <bunsan/binlogs/directory_log/detail.hpp>
#include <bunsan/binlogs/directory_log/Error.hpp>
#include <bunsan/binlogs/LogFactory.hpp>

namespace bunsan {
namespace binlogs {
namespace directory_log {

LogReader::LogReader(const boost::filesystem::path &path):
    paths_(detail::listDir(path)), next_(0)
{
    update();
}

bool LogReader::read(google::protobuf::Message &message)
{
    try {
        while (update()) {
            if (logReader_->read(message)) {
                return true;
            }
            BOOST_ASSERT(logReader_->eof());
        }
        return false;
    } catch (std::exception &) {
        BOOST_THROW_EXCEPTION(LogReaderReadError() <<
                              enable_nested_current());
    }
}

const MessageType *LogReader::nextMessageType()
{
    try {
        while (update()) {
            if (const MessageType *const msgType = logReader_->nextMessageType()) {
                return msgType;
            }
            BOOST_ASSERT(logReader_->eof());
        }
        return nullptr;
    } catch (std::exception &) {
        BOOST_THROW_EXCEPTION(LogReaderNextMessageTypeError() <<
                              enable_nested_current());
    }
}

void LogReader::close()
{
    if (logReader_) {
        logReader_->close();
        logReader_.reset();
    }
    paths_.clear();
    next_ = paths_.size();
}

LogReader::State LogReader::state() const
{
    try {
        if (logReader_) {
            switch (const State st = logReader_->state()) {
            case State::kBad:
                return State::kFail;
            default:
                return st;
            }
        }
        BOOST_ASSERT(next_ == paths_.size());
        return State::kEof;
    } catch (std::exception &) {
        BOOST_THROW_EXCEPTION(LogReaderStateError() <<
                              enable_nested_current());
    }
}

const binlogs::MessageTypePool &LogReader::messageTypePool() const
{
    try {
        if (logReader_) {
            return logReader_->messageTypePool();
        }
        BOOST_ASSERT(eof());
        BOOST_THROW_EXCEPTION(ClosedReaderError());
    } catch (std::exception &) {
        BOOST_THROW_EXCEPTION(LogReaderMessageTypePoolError() <<
                              enable_nested_current());
    }
}

void LogReader::fastCheck()
{
    try {
        // note: this function is always successful (except close() failures)
        close();
    } catch (std::exception &) {
        BOOST_THROW_EXCEPTION(LogReaderFastCheckError() <<
                              enable_nested_current());
    }
}

bool LogReader::update()
{
    if (logReader_ && !logReader_->usable()) {
        logReader_.reset();
    }
    if (!logReader_) {
        if (next_ < paths_.size()) {
            logReader_ = openReadOnly(paths_[next_++]);
        } else {
            return false;
        }
    }
    return true;
}

}
}
}
