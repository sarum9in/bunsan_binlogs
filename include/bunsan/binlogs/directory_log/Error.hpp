#pragma once

#include <bunsan/binlogs/Error.hpp>

namespace bunsan {
namespace binlogs {
namespace directory_log {

struct LogReaderError: virtual binlogs::LogReaderError {};
struct LogReaderReadError: virtual LogReaderError {};
struct LogReaderNextMessageTypeError: virtual LogReaderError {};
struct LogReaderCloseError: virtual LogReaderError {};
struct LogReaderStateError: virtual LogReaderError {};
struct LogReaderMessageTypePoolError: virtual LogReaderError {};
struct LogReaderFastCheckError: virtual LogReaderError {};

struct LogWriterError: virtual NamedLogWriterError {};
struct LogWriterWriteError: virtual LogWriterError {};
struct LogWriterMessageTypePoolError: virtual LogReaderError {};
struct LogWriterOpenError: virtual LogWriterError, virtual NamedLogWriterOpenError {};
struct LogWriterRotateError: virtual LogWriterError, virtual NamedLogWriterRotateError {};
struct LogWriterReopenError: virtual LogWriterError, virtual NamedLogWriterReopenError {};
struct LogWriterAppendError: virtual LogWriterError, virtual NamedLogWriterAppendError {};

struct LogWriterFormatError: virtual LogWriterError {};
struct IsNotADirectoryError: virtual LogWriterFormatError {};
struct PathExistsError: virtual LogWriterFormatError {};
struct TooManyLogFilesError: virtual LogWriterFormatError {};

}
}
}
