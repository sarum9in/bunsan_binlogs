#include <bunsan/binlogs/v1/NamedLogWriter.hpp>
#include <bunsan/binlogs/detail/make_unique.hpp>
#include <bunsan/binlogs/io/file/open.hpp>
#include <bunsan/binlogs/io/filter/gzip.hpp>

#include <boost/filesystem/operations.hpp>

namespace bunsan {
namespace binlogs {
namespace v1 {

NamedLogWriter::NamedLogWriter(const FileOpener &openFile):
    openFile_(openFile) {}

bool NamedLogWriter::Init(const Header &header, std::string *error)
{
    if (!pool_.Init(header, error)) {
        return false;
    }
    headerData_ = detail::make_unique<HeaderData>();
    *headerData_->mutable_proto() = header.proto;
    for (const std::string &type: header.types) {
        headerData_->add_types()->set_name(type);
    }
    return true;
}

bool NamedLogWriter::write(
    const std::string &typeName,
    const google::protobuf::Message &message,
    std::string *error)
{
    if (hasOutput()) {
        BOOST_ASSERT(headerData_);
        state_ = write(&typeName, message, error);
        switch (state_) {
        case State::kOk:
            return true;
        case State::kBad:
        case State::kEof:
            closeOutput();
        case State::kFail:
            return false;
        }
        BOOST_ASSERT(false);
    } else {
        if (error) {
            *error = "NamedLogWriter is closed.";
        }
        return false;
    }
}

bool NamedLogWriter::close(std::string *error)
{
    state_ = State::kEof;
    return closeOutput(error);
}

NamedLogWriter::State NamedLogWriter::state() const
{
    return state_;
}

boost::filesystem::path NamedLogWriter::path() const
{
    return path_;
}

bool NamedLogWriter::open(const boost::filesystem::path &path, std::string *error)
{
    if (hasOutput()) {
        if (error) {
            *error = "NamedLogWriter is already opened.";
        }
        return false;
    }
    path_ = path;
    auto output = openFile_(path_, error);
    if (!output) {
        return false;
    }
    setOutput(std::move(output));
    state_ = write(nullptr, *headerData_, error);
    if (state_ != State::kOk) {
        state_ = State::kBad;
        closeOutput();
        if (error) {
            *error = "Unable to write header: " + *error;
        }
        return false;
    }
    return true;
}


bool NamedLogWriter::reopen(std::string *error)
{
    return reopen(path_, error);
}

bool NamedLogWriter::reopen(const boost::filesystem::path &newPath, std::string *error)
{
    if (!hasOutput()) {
        if (error) {
            *error = "NamedLogWriter is not opened.";
        }
        return false;
    }
    if (!close(error)) {
        return false;
    }
    return open(newPath, error);
}

bool NamedLogWriter::rotate(const boost::filesystem::path &renameTo, std::string *error)
{
    if (!hasOutput()) {
        if (error) {
            *error = "NamedLogWriter is not opened.";
        }
        return false;
    }
    if (!close(error)) {
        return false;
    }
    boost::system::error_code ec;
    boost::filesystem::rename(path_, renameTo, ec);
    if (ec) {
        if (error) {
            *error = ec.message();
        }
        return false;
    }
    return open(path_, error);
}

const v1::MessageTypePool &NamedLogWriter::messageTypePool__() const
{
    return pool_;
}

}
}
}
