#include <bunsan/binlogs/LogFactory.hpp>

#include <bunsan/binlogs/io/file/open.hpp>
#include <bunsan/binlogs/io/filter/gzip.hpp>
#include <bunsan/binlogs/v1/format.hpp>
#include <bunsan/binlogs/v1/LogReader.hpp>
#include <bunsan/binlogs/v1/LogWriter.hpp>
#include <bunsan/binlogs/v1/NamedLogWriter.hpp>

#include <bunsan/binlogs/detail/make_unique.hpp>

#include <google/protobuf/io/coded_stream.h>

#include <boost/assert.hpp>
#include <boost/format.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace bunsan {
namespace binlogs {

namespace {

bool writeMagic(io::WriteBuffer &output, std::string *error)
{
    google::protobuf::io::CodedOutputStream outp(&output);
    outp.WriteRaw(&v1::MAGIC_FORMAT, static_cast<int>(v1::MAGIC_FORMAT.size()));
    if (outp.HadError()) {
        if (error) {
            *error = "Unable to write format magic.";
        }
        return false;
    }
    return true;
}

std::unique_ptr<io::ReadBuffer> openFileReadOnly(const boost::filesystem::path &path, std::string *error)
{
    std::unique_ptr<io::ReadBuffer> buffer = io::file::openReadOnly(path, error);
    if (!buffer) return buffer;
    buffer = io::filter::gzip::open(std::move(buffer), error);
    return buffer;
}

std::unique_ptr<io::WriteBuffer> openFileWriteOnly(const boost::filesystem::path &path, std::string *error)
{
    std::unique_ptr<io::WriteBuffer> buffer = io::file::openWriteOnly(path, error);
    if (!buffer) return buffer;
    buffer = io::filter::gzip::open(std::move(buffer), error);
    if (!writeMagic(*buffer, error)) {
        buffer.reset();
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
    {
        google::protobuf::io::CodedInputStream inp(input.get());
        if (!inp.ReadRaw(&format, static_cast<int>(format.size()))) {
            if (error) {
                *error = "Unable to read format magic.";
            }
            return nullptr;
        }
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
    auto input = openFileReadOnly(path, error);
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
    std::unique_ptr<LogWriter> logWriter = detail::make_unique<v1::LogWriter>(std::move(output));
    if (!logWriter->Init(header, error)) {
        logWriter.reset();
    }
    return logWriter;
}

std::unique_ptr<NamedLogWriter> openWriteOnly(
    const boost::filesystem::path &path,
    const Header &header,
    std::string *error)
{
    std::unique_ptr<NamedLogWriter> logWriter =
        detail::make_unique<v1::NamedLogWriter>(openFileWriteOnly);
    if (!logWriter->Init(header, error)) {
        return nullptr;
    }
    if (!logWriter->open(path, error)) {
        return nullptr;
    }
    return logWriter;
}

}
}
