#include "bunsan/binlogs/LogFactory.hpp"

#include "bunsan/binlogs/v1/LogReader.hpp"
#include "bunsan/binlogs/v1/LogWriter.hpp"

#include "bunsan/binlogs/detail/make_unique.hpp"

#include <boost/assert.hpp>
#include <boost/format.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/string_generator.hpp>

namespace bunsan {
namespace binlogs {

namespace {
const boost::uuids::uuid MAGIC_FORMAT_V1 = boost::uuids::string_generator()("f6a03dc0-eaf5-11e2-91e2-0800200c9a66");
}

std::unique_ptr<LogReader> openReadOnly(
    std::unique_ptr<google::protobuf::io::CodedInputStream> &&input,
    std::string *error)
{
    BOOST_ASSERT(input);
    boost::uuids::uuid format;
    if (!input->ReadRaw(&format, static_cast<int>(format.size()))) {
        if (error) {
            *error = "Unable to read format magic.";
        }
        return nullptr;
    }

    std::unique_ptr<LogReader> logReader;
    if (format == MAGIC_FORMAT_V1) {
        logReader = detail::make_unique<v1::LogReader>(std::move(input));
    } else {
        if (error) {
            *error = str(boost::format("Unknown format {%1%}.") % format);
        }
    }
    if (logReader && !logReader->readHeader(error)) {
        logReader.reset();
    }
    return logReader;
}

std::unique_ptr<LogWriter> openWriteOnly(
    std::unique_ptr<google::protobuf::io::CodedOutputStream> &&output,
    const Header &header,
    std::string *error)
{
    BOOST_ASSERT(output);
    BOOST_ASSERT(!output->HadError());
    output->WriteRaw(&MAGIC_FORMAT_V1, static_cast<int>(MAGIC_FORMAT_V1.size()));
    if (output->HadError()) {
        if (error) {
            *error = "Unable to write format magic.";
        }
        return nullptr;
    }
    std::unique_ptr<LogWriter> logWriter = detail::make_unique<v1::LogWriter>(std::move(output));
    if (!logWriter->writeHeader(header, error)) {
        logWriter.reset();
    }
    return logWriter;
}

}
}