#include "bunsan/binlogs/v1/LogWriter.hpp"
#include "bunsan/binlogs/detail/make_unique.hpp"

#include <google/protobuf/io/coded_stream.h>

#include <boost/assert.hpp>
#include <boost/format.hpp>

namespace bunsan {
namespace binlogs {
namespace v1 {

LogWriter::LogWriter(std::unique_ptr<io::WriteBuffer> &&output):
    output_(std::move(output))
{
    BOOST_ASSERT(output_);
}

bool LogWriter::Init(const Header &header, std::string *error)
{
    if (!pool_.Init(header, error)) {
        return false;
    }
    headerData_ = detail::make_unique<HeaderData>();
    *headerData_->mutable_proto() = header.proto;
    for (const std::string &type: header.types) {
        headerData_->add_types()->set_name(type);
    }
    state_ = write(nullptr, *headerData_, error);
    if (state_ != State::kOk) {
        state_ = State::kBad;
        output_.reset();
        if (error) {
            *error = "Unable to write header: " + *error;
        }
        return false;
    }
    return true;
}

bool LogWriter::write(const std::string &typeName,
                      const google::protobuf::Message &message,
                      std::string *error)
{
    if (output_) {
        BOOST_ASSERT(headerData_);
        // FIXME We should check that message has type defined in header.
        // Or should we create template-based wrapper for this class?
        state_ = write(&typeName, message, error);
        switch (state_) {
        case State::kOk:
            return true;
        case State::kBad:
        case State::kEof:
            output_.reset();
        case State::kFail:
            return false;
        }
    }
    return false;
}

bool LogWriter::close(std::string *error)
{
    state_ = State::kEof;
    const bool ret = output_->close();
    if (!ret) {
        BOOST_VERIFY(output_->error(error));
    }
    output_.reset();
    return ret;
}

LogWriter::State LogWriter::state() const
{
    return state_;
}

io::WriteBuffer *LogWriter::output__()
{
    return output_.get();
}

const v1::MessageTypePool &LogWriter::messageTypePool__() const
{
    return pool_;
}

}
}
}
