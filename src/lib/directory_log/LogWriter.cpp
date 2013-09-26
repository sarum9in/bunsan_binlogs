#include <bunsan/binlogs/directory_log/LogWriter.hpp>

#include <bunsan/binlogs/directory_log/detail.hpp>
#include <bunsan/binlogs/Error.hpp>
#include <bunsan/binlogs/LogFactory.hpp>

#include <boost/assert.hpp>
#include <boost/filesystem/operations.hpp>

namespace bunsan {
namespace binlogs {
namespace directory_log {

LogWriter::LogWriter(const Header &header):
    header_(header) {}

void LogWriter::write(const std::string &typeName,
                      const google::protobuf::Message &message)
{
    try {
        if (!logWriter_) {
            BOOST_THROW_EXCEPTION(ClosedWriterError());
        }
        logWriter_->write(typeName, message);
    } catch (std::exception &) {
        BOOST_THROW_EXCEPTION(LogWriterWriteError() <<
                              enable_nested_current());
    }
}

const binlogs::MessageTypePool &LogWriter::messageTypePool() const
{
    try {
        if (!logWriter_) {
            BOOST_THROW_EXCEPTION(ClosedWriterError());
        }
        return logWriter_->messageTypePool();
    } catch (std::exception &) {
        BOOST_THROW_EXCEPTION(LogWriterMessageTypePoolError() <<
                              enable_nested_current());
    }
}

void LogWriter::close()
{
    if (logWriter_)
        logWriter_->close();
    logWriter_.reset();
}

LogWriter::State LogWriter::state() const
{
    if (logWriter_)
        return logWriter_->state();
    return State::kEof;
}

boost::filesystem::path LogWriter::path() const
{
    return path_;
}

void LogWriter::open(const boost::filesystem::path &path)
{
    try {
        if (logWriter_) {
            BOOST_THROW_EXCEPTION(OpenedWriterError() <<
                                  OpenedWriterError::Path(path_));
        }
        if (boost::filesystem::exists(path)) {
            BOOST_THROW_EXCEPTION(PathExistsError());
        }
        BOOST_VERIFY(boost::filesystem::create_directory(path));
        append(path);
    } catch (std::exception &) {
        BOOST_THROW_EXCEPTION(LogWriterOpenError() <<
                              enable_nested_current() <<
                              LogWriterOpenError::Path(path));
    }
}

void LogWriter::append(const boost::filesystem::path &path)
{
    try {
        if (logWriter_) {
            BOOST_THROW_EXCEPTION(OpenedWriterError() <<
                                  OpenedWriterError::Path(path_));
        }
        path_ = path;
        if (!boost::filesystem::is_directory(path_)) {
            BOOST_THROW_EXCEPTION(IsNotADirectoryError());
        }
        const boost::filesystem::path nextPath = detail::nextPath(path_);
        logWriter_ = openWriteOnly(nextPath, header_);
    } catch (std::exception &) {
        BOOST_THROW_EXCEPTION(LogWriterAppendError() <<
                              enable_nested_current() <<
                              LogWriterAppendError::Path(path));
    }
}

void LogWriter::reopen()
{
    try {
        reopen(path_);
    } catch (std::exception &) {
        BOOST_THROW_EXCEPTION(LogWriterReopenError() <<
                              enable_nested_current());
    }
}

void LogWriter::reopen(const boost::filesystem::path &newPath)
{
    try {
        if (!logWriter_) {
            BOOST_THROW_EXCEPTION(ClosedWriterError());
        }
        close();
        open(newPath);
    } catch (std::exception &) {
        BOOST_THROW_EXCEPTION(LogWriterReopenError() <<
                              enable_nested_current() <<
                              LogWriterReopenError::Path(newPath));
    }
}

void LogWriter::rotate(const boost::filesystem::path &renameTo)
{
    try {
        if (!logWriter_) {
            BOOST_THROW_EXCEPTION(ClosedWriterError());
        }
        close();
        boost::filesystem::rename(path_, renameTo);
        open(path_);
    } catch (std::exception &) {
        BOOST_THROW_EXCEPTION(LogWriterRotateError() <<
                              enable_nested_current() <<
                              LogWriterRotateError::Path(renameTo));
    }
}

}
}
}
