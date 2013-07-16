#include "bunsan/binlogs/v1/LogWriter.hpp"
#include "bunsan/binlogs/detail/make_unique.hpp"

#include <boost/assert.hpp>

namespace bunsan {
namespace binlogs {
namespace v1 {

LogWriter::LogWriter(std::unique_ptr<google::protobuf::io::CodedOutputStream> &&output):
    output_(std::move(output))
{
    BOOST_ASSERT(output_);
    BOOST_ASSERT(!output_->HadError());
}

bool LogWriter::writeHeader(
    const google::protobuf::FileDescriptorSet &fileDescriptorSet,
    std::string *error)
{
    headerData_ = detail::make_unique<HeaderData>();
    *headerData_->mutable_entry()->mutable_proto() = fileDescriptorSet;
    headerData_->mutable_entry()->set_type_name(message.GetTypeName());
    if (!write_(*headerData_, error)) {
        state_ = State::kBad;
        output_.reset();
        if (error) {
            *error = "Unable to write header: " + *error;
        }
        return false;
    }
    return true;
}

bool LogWriter::write(const google::protobuf::Message &message, std::string *error)
{
    if (output_) {
        BOOST_ASSERT(headerData_);
        // FIXME We should check that message has type defined in header.
        // Or should we create template-based wrapper for this class?
        return write_(message, error);
    } else {
        return false;
    }
}

bool LogWriter::write_(const google::protobuf::Message &message, std::string *error)
{
    if (output_) {
        std::string error_;
        if (!error) {
            error = &error_;
        }

        BOOST_ASSERT(!output_->HadError());
        const google::protobuf::uint32 messageSize = message.ByteSize();
        output_->WriteLittleEndian32(messageSize);
        if (output_->HadError()) {
            state_ = State::kBad;
            output_.reset();
            *error = "Unable to write message size.";
            return false;
        }
        message.SerializeWithCachedSizes(output_.get());
        if (output_->HadError()) {
            state_ = State::kBad;
            output_.reset();
            *error = "Unable to write message.";
            return false;
        }
        BOOST_ASSERT(!output_->HadError());
        state_ = State::kOk;
        return true;
    } else {
        return false;
    }
}

bool LogWriter::close(std::string */*error*/)
{
    state_ = State::kEof;
    output_.reset();
    return true;
}

LogWriter::State LogWriter::state() const
{
    return state_;
}

}
}
}
