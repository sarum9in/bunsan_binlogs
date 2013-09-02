#include <bunsan/binlogs/v1/NamedLogWriter.hpp>

#include <bunsan/binlogs/detail/files.hpp>
#include <bunsan/binlogs/detail/format.hpp>
#include <bunsan/binlogs/detail/make_unique.hpp>
#include <bunsan/binlogs/io/file/open.hpp>
#include <bunsan/binlogs/io/filter/gzip.hpp>
#include <bunsan/binlogs/v1/Error.hpp>
#include <bunsan/binlogs/v1/format.hpp>
#include <bunsan/binlogs/v1/LogReader.hpp>

#include <boost/filesystem/operations.hpp>

namespace bunsan {
namespace binlogs {
namespace v1 {

NamedLogWriter::NamedLogWriter(const FileOpener &openFile, const Header &header):
    openFile_(openFile), pool_(header)
{
    headerData_ = detail::make_unique<HeaderData>();
    *headerData_->mutable_proto() = header.proto;
    for (const std::string &type: header.types) {
        headerData_->add_types()->set_name(type);
    }
}

void NamedLogWriter::write(
    const std::string &typeName,
    const google::protobuf::Message &message)
{
    if (!hasOutput()) {
        BOOST_THROW_EXCEPTION(ClosedWriterError());
    }
    BOOST_ASSERT(headerData_);
    try {
        write(&typeName, message);
    } catch (std::exception &e) {
        if (recoverable(e)) {
            state_ = State::kFail;
        } else {
            state_ = State::kBad;
            try { closeOutput(); } catch (std::exception &) {}
        }
        BOOST_THROW_EXCEPTION(UnableToWriteMessageError().enable_nested_current());
    }
}

void NamedLogWriter::close()
{
    state_ = State::kEof;
    closeOutput();
}

NamedLogWriter::State NamedLogWriter::state() const
{
    return state_;
}

boost::filesystem::path NamedLogWriter::path() const
{
    return path_;
}

void NamedLogWriter::open_(
    const boost::filesystem::path &path,
    const bool append)
{
    if (hasOutput()) {
        BOOST_THROW_EXCEPTION(OpenedWriterError());
    }
    try {
        path_ = path;
        if (append) {
            auto input = detail::openFileReadOnly(path_);
            boost::uuids::uuid format;
            detail::readFormatMagic(*input, format);
            if (format != MAGIC_FORMAT) {
                BOOST_THROW_EXCEPTION(IncompatibleFormatError() <<
                                      IncompatibleFormatError::Format(format));
            }
            auto logReader = detail::make_unique<LogReader>(std::move(input));
            if (logReader->messageTypePool().header() != pool_.header()) {
                BOOST_THROW_EXCEPTION(IncompatibleHeaderError());
            }
            logReader->fastCheck();
        }
        setOutput(openFile_(path_, append));
        if (append) {
            writeContinue();
        } else {
            try {
                write(nullptr, *headerData_);
            } catch (std::exception &) {
                BOOST_THROW_EXCEPTION(UnableToWriteHeaderError().enable_nested_current());
            }
        }
    } catch (std::exception &) {
        if (boost::exception *const e = boost::current_exception_cast<boost::exception>()) {
            *e << Error::Path(path_);
        }
        state_ = State::kBad;
        try { closeOutput(); } catch (std::exception &) {}
        throw;
    }
}

void NamedLogWriter::open(const boost::filesystem::path &path)
{
    try {
        open_(path, false);
    } catch (std::exception &) {
        BOOST_THROW_EXCEPTION(NamedLogWriterOpenError().enable_nested_current());
    }
}

void NamedLogWriter::append(const boost::filesystem::path &path)
{
    try {
        open_(path, true);
    } catch (std::exception &) {
        BOOST_THROW_EXCEPTION(NamedLogWriterAppendError().enable_nested_current());
    }
}

void NamedLogWriter::reopen()
{
    try {
        reopen(path_);
    } catch (std::exception &) {
        BOOST_THROW_EXCEPTION(NamedLogWriterReopenError().enable_nested_current());
    }
}

void NamedLogWriter::reopen(const boost::filesystem::path &newPath)
{
    try {
        if (!hasOutput()) {
            BOOST_THROW_EXCEPTION(ClosedWriterError());
        }
        close();
        open(newPath);
    } catch (std::exception &) {
        BOOST_THROW_EXCEPTION(NamedLogWriterReopenError().enable_nested_current());
    }
}

void NamedLogWriter::rotate(const boost::filesystem::path &renameTo)
{
    try {
        if (!hasOutput()) {
            BOOST_THROW_EXCEPTION(ClosedWriterError());
        }
        close();
        boost::filesystem::rename(path_, renameTo);
        open(path_);
    } catch (std::exception &) {
        BOOST_THROW_EXCEPTION(NamedLogWriterRotateError().enable_nested_current());
    }
}

const v1::MessageTypePool &NamedLogWriter::messageTypePool__() const
{
    return pool_;
}

}
}
}
