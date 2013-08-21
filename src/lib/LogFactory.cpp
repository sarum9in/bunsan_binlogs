#include <bunsan/binlogs/LogFactory.hpp>

#include <bunsan/binlogs/detail/files.hpp>
#include <bunsan/binlogs/detail/format.hpp>
#include <bunsan/binlogs/detail/make_unique.hpp>
#include <bunsan/binlogs/v1/format.hpp>
#include <bunsan/binlogs/v1/LogReader.hpp>
#include <bunsan/binlogs/v1/LogWriter.hpp>
#include <bunsan/binlogs/v1/NamedLogWriter.hpp>

#include <boost/assert.hpp>
#include <boost/format.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace bunsan {
namespace binlogs {

namespace current = v1;

namespace {

bool writeMagic(io::WriteBuffer &output, std::string *error)
{
    return detail::writeFormatMagic(output, current::MAGIC_FORMAT, error);
}

std::unique_ptr<io::WriteBuffer> openFileWriteOnly(
    const boost::filesystem::path &path,
    const bool append,
    std::string *error)
{
    std::unique_ptr<io::WriteBuffer> buffer;
    if (append) {
        buffer = detail::openFileAppendOnly(path, error);
    } else {
        buffer = detail::openFileWriteOnly(path, error);
    }
    if (!buffer) return nullptr;
    if (!append) {
        if (!writeMagic(*buffer, error)) return nullptr;
    }
    return buffer;
}

}

std::unique_ptr<LogReader> openReadOnly(
    std::unique_ptr<io::ReadBuffer> &&input,
    std::string *error)
{
    BOOST_ASSERT(input);
    boost::uuids::uuid format;
    if (!detail::readFormatMagic(*input, format, error)) {
        return nullptr;
    }

    std::unique_ptr<LogReader> logReader;
    if (format == v1::MAGIC_FORMAT) {
        logReader = detail::make_unique<v1::LogReader>(std::move(input));
    } else {
        if (error) {
            *error = str(boost::format("Unknown format {%1%}.") % format);
        }
    }
    if (logReader && !logReader->Init(error)) {
        logReader.reset();
    }
    return logReader;
}

std::unique_ptr<LogReader> openReadOnly(
    const boost::filesystem::path &path,
    std::string *error)
{
    auto input = detail::openFileReadOnly(path, error);
    if (!input) {
        if (error) {
            *error = str(boost::format("%1%: %2%") % path % *error);
        }
        return nullptr;
    }
    auto logReader = openReadOnly(std::move(input), error);
    if (!logReader && error) {
        *error = str(boost::format("%1%: %2%") % path % *error);
    }
    return logReader;
}

std::unique_ptr<LogWriter> openWriteOnly(
    std::unique_ptr<io::WriteBuffer> &&output,
    const Header &header,
    std::string *error)
{
    BOOST_ASSERT(output);
    if (!writeMagic(*output, error)) {
        return nullptr;
    }
    std::unique_ptr<LogWriter> logWriter =
        detail::make_unique<current::LogWriter>(std::move(output));
    if (!logWriter->Init(header, error)) {
        logWriter.reset();
    }
    return logWriter;
}

std::unique_ptr<NamedLogWriter> newWriter(
    const Header &header,
    std::string *error)
{
    std::unique_ptr<NamedLogWriter> logWriter =
        detail::make_unique<current::NamedLogWriter>(openFileWriteOnly);
    if (!logWriter->Init(header, error)) {
        return nullptr;
    }
    return logWriter;
}

std::unique_ptr<NamedLogWriter> openWriteOnly(
    const boost::filesystem::path &path,
    const Header &header,
    std::string *error)
{
    std::unique_ptr<NamedLogWriter> logWriter = newWriter(header, error);
    if (!logWriter) {
        return nullptr;
    }
    if (!logWriter->open(path, error)) {
        return nullptr;
    }
    return logWriter;
}

std::unique_ptr<NamedLogWriter> openAppendOnly(
    const boost::filesystem::path &path,
    const Header &header,
    std::string *error)
{
    std::unique_ptr<NamedLogWriter> logWriter = newWriter(header, error);
    if (!logWriter) {
        return nullptr;
    }
    if (!logWriter->append(path, error)) {
        return nullptr;
    }
    return logWriter;
}

}
}
