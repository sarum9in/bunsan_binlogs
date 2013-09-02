#include <bunsan/binlogs/v1/LogWriter.hpp>

#include <bunsan/binlogs/detail/make_unique.hpp>
#include <bunsan/binlogs/v1/Error.hpp>

#include <google/protobuf/io/coded_stream.h>

#include <boost/assert.hpp>

namespace bunsan {
namespace binlogs {
namespace v1 {

LogWriter::LogWriter(
    const Header &header,
    std::unique_ptr<io::WriteBuffer> &&output):
    pool_(header)
{
    setOutput(std::move(output));
    headerData_ = detail::make_unique<HeaderData>();
    *headerData_->mutable_proto() = header.proto;
    for (const std::string &type: header.types) {
        headerData_->add_types()->set_name(type);
    }
    try {
        write(nullptr, *headerData_);
    } catch (std::exception &) {
        state_ = State::kBad;
        try { closeOutput(); } catch (std::exception &) {}
        BOOST_THROW_EXCEPTION(UnableToWriteHeaderError().enable_nested_current());
    }
}

void LogWriter::write(const std::string &typeName,
                      const google::protobuf::Message &message)
{
    if (!hasOutput()) {
        BOOST_THROW_EXCEPTION(ClosedWriterError());
    }
    BOOST_ASSERT(headerData_);
    // FIXME We should check that message has type defined in header.
    // Or should we create template-based wrapper for this class?
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

void LogWriter::close()
{
    state_ = State::kEof;
    closeOutput();
}

LogWriter::State LogWriter::state() const
{
    return state_;
}

const v1::MessageTypePool &LogWriter::messageTypePool__() const
{
    return pool_;
}

}
}
}
