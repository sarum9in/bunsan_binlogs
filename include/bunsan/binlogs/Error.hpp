#pragma once

#include <bunsan/error.hpp>
#include <bunsan/filesystem/error.hpp>

#include <boost/uuid/uuid.hpp>

namespace bunsan {
namespace binlogs {

struct Error: virtual bunsan::error
{
    typedef bunsan::error::message Message;
    typedef bunsan::filesystem::error::path Path;
};

struct LogReaderError: virtual Error {};
struct ClosedReaderError: virtual Error {};

struct LogWriterError: virtual Error {};
struct OpenedWriterError: virtual LogWriterError {};
struct ClosedWriterError: virtual LogWriterError {};
struct NamedLogWriterError: virtual LogWriterError {};
struct NamedLogWriterOpenError: virtual NamedLogWriterError {};
struct NamedLogWriterRotateError: virtual NamedLogWriterError {};
struct NamedLogWriterReopenError: virtual NamedLogWriterError {};
struct NamedLogWriterAppendError: virtual NamedLogWriterError {};

struct FormatError: virtual Error {
    typedef boost::error_info<struct TagFormat, boost::uuids::uuid> Format;
};
struct UnableToReadFormatMagicError: virtual FormatError {};
struct UnableToWriteFormatMagicError: virtual FormatError {};
struct UnableToWriteContinueMagicError: virtual FormatError {};
struct UnknownFormatError: virtual FormatError {};

struct DescriptorError: virtual Error {
    typedef boost::error_info<struct TagDescriptorType, std::string> DescriptorType;
};
struct UnableToAllocateDescriptorError: virtual DescriptorError {};
struct UnableToAllocatePrototypeError: virtual DescriptorError {};
struct DescriptorDatabaseError: virtual DescriptorError {};
struct FileAlreadyInDatabaseError: virtual DescriptorDatabaseError {};

struct HeaderError: virtual FormatError {};
struct UnableToParseHeaderError: virtual HeaderError {};

struct FooterError: virtual FormatError {};
struct UnableToReadFooterError: virtual FooterError {};
struct InvalidFooterError: virtual FooterError {};

struct MessageError: virtual Error {
    typedef boost::error_info<struct TagMessageTypeName, std::string> MessageTypeName;
};
struct MessageTypeError: virtual MessageError {};
struct UnknownMessageTypeError: virtual MessageTypeError {};
struct MessageFormatError: virtual MessageError, virtual FormatError {};
struct UnableToReadMessageError: virtual MessageFormatError {};
struct UninitializedMessageError: virtual MessageError {
    typedef boost::error_info<struct TagUninitializedFields, std::vector<std::string>> UninitializedFields;
};

}
}

namespace boost {

std::string to_string(const bunsan::binlogs::FormatError::Format &format);
std::string to_string(const bunsan::binlogs::UninitializedMessageError::UninitializedFields &fields);

}
