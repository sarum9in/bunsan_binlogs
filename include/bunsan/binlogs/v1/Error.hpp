#pragma once

#include <bunsan/binlogs/Error.hpp>

namespace bunsan {
namespace binlogs {
namespace v1 {

struct Error: virtual binlogs::Error {};
struct FormatError: virtual binlogs::FormatError, virtual Error {};
struct MessageFormatError: virtual binlogs::MessageFormatError, virtual Error {};

struct MessageTypeError: virtual binlogs::MessageTypeError, virtual Error {};
struct UnknownMessageTypeError: virtual binlogs::MessageTypeError, virtual MessageTypeError {};
struct MessageTypeIdError: virtual MessageTypeError {
    typedef boost::error_info<struct TagMessageTypeId, std::size_t> MessageTypeId;
};
struct UnknownMessageTypeIdError: virtual MessageTypeIdError {};

struct ReadError: virtual FormatError {};
struct ReadLittleEndian32Error: virtual ReadError {};
struct UnableToReadMessageSizeError: virtual ReadLittleEndian32Error {};
struct UnableToReadMessageTypeError: virtual ReadLittleEndian32Error {};
struct UnableToSkipUnreadBytesError: virtual ReadError {};
struct UnableToSkipMessageError: virtual ReadError {};

struct MessageReadError: virtual ReadError, virtual MessageFormatError {
    typedef boost::error_info<struct TagMessageSize, std::size_t> MessageSize;
};

struct WriteError: virtual Error {};
struct WriteLittleEndian32Error: virtual WriteError {};
struct UnableToWriteMessageTypeError: virtual WriteLittleEndian32Error {};
struct UnableToWriteMessageSizeError: virtual WriteLittleEndian32Error {};
struct UnableToWriteHeaderError: virtual WriteError {};
struct UnableToWriteMessageError: virtual WriteError {};
struct UnableToWriteFooterError: virtual WriteError {};
struct UnableToCloseOutputError: virtual WriteError {};

struct IncompatibleFormatError: virtual FormatError {};
struct IncompatibleHeaderError: virtual FormatError {};

typedef boost::error_info<struct TagRecoverable, bool> Recoverable;

template <typename Exception>
inline bool recoverable(const Exception &e)
{
    const bool *const r = boost::get_error_info<Recoverable>(e);
    return r && *r;
}

}
}
}
