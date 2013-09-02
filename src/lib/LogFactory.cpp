#include <bunsan/binlogs/LogFactory.hpp>

#include <bunsan/binlogs/detail/files.hpp>
#include <bunsan/binlogs/detail/format.hpp>
#include <bunsan/binlogs/detail/make_unique.hpp>
#include <bunsan/binlogs/Error.hpp>
#include <bunsan/binlogs/v1/format.hpp>
#include <bunsan/binlogs/v1/LogReader.hpp>
#include <bunsan/binlogs/v1/LogWriter.hpp>
#include <bunsan/binlogs/v1/NamedLogWriter.hpp>

#include <boost/assert.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace bunsan {
namespace binlogs {

namespace current = v1;

namespace {

void writeMagic(io::WriteBuffer &output)
{
    detail::writeFormatMagic(output, current::MAGIC_FORMAT);
}

std::unique_ptr<io::WriteBuffer> openFileWriteOnly(
    const boost::filesystem::path &path,
    const bool append)
{
    std::unique_ptr<io::WriteBuffer> buffer;
    if (append) {
        buffer = detail::openFileAppendOnly(path);
    } else {
        buffer = detail::openFileWriteOnly(path);
    }
    BOOST_ASSERT(buffer);
    if (!append) {
        writeMagic(*buffer);
    }
    return buffer;
}

}

std::unique_ptr<LogReader> openReadOnly(std::unique_ptr<io::ReadBuffer> &&input)
{
    BOOST_ASSERT(input);
    boost::uuids::uuid format;
    detail::readFormatMagic(*input, format);

    std::unique_ptr<LogReader> logReader;
    if (format == v1::MAGIC_FORMAT) {
        logReader = detail::make_unique<v1::LogReader>(std::move(input));
    } else {
        BOOST_THROW_EXCEPTION(UnknownFormatError() <<
                              UnknownFormatError::Format(format));
    }
    return logReader;
}

std::unique_ptr<LogReader> openReadOnly(const boost::filesystem::path &path)
{
    try {
        auto input = detail::openFileReadOnly(path);
        auto logReader = openReadOnly(std::move(input));
        return logReader;
    } catch (boost::exception &e) {
        e << Error::Path(path);
        throw;
    }
}

std::unique_ptr<LogWriter> openWriteOnly(
    std::unique_ptr<io::WriteBuffer> &&output,
    const Header &header)
{
    BOOST_ASSERT(output);
    writeMagic(*output);
    std::unique_ptr<LogWriter> logWriter =
        detail::make_unique<current::LogWriter>(
            header, std::move(output));
    return logWriter;
}

std::unique_ptr<NamedLogWriter> newWriter(const Header &header)
{
    return detail::make_unique<current::NamedLogWriter>(openFileWriteOnly, header);
}

std::unique_ptr<NamedLogWriter> openWriteOnly(
    const boost::filesystem::path &path,
    const Header &header)
{
    try {
        std::unique_ptr<NamedLogWriter> logWriter = newWriter(header);
        logWriter->open(path);
        return logWriter;
    } catch (boost::exception &e) {
        e << Error::Path(path);
        throw;
    }
}

std::unique_ptr<NamedLogWriter> openAppendOnly(
    const boost::filesystem::path &path,
    const Header &header)
{
    try {
        std::unique_ptr<NamedLogWriter> logWriter = newWriter(header);
        logWriter->append(path);
        return logWriter;
    } catch (boost::exception &e) {
        e << Error::Path(path);
        throw;
    }
}

}
}
