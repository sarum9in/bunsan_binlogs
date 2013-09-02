#include <bunsan/binlogs/detail/format.hpp>

#include <bunsan/binlogs/Error.hpp>

#include <google/protobuf/io/coded_stream.h>

namespace bunsan {
namespace binlogs {
namespace detail {

void readFormatMagic(
    io::ReadBuffer &input,
    boost::uuids::uuid &format)
{
    google::protobuf::io::CodedInputStream inp(&input);
    if (!inp.ReadRaw(&format, static_cast<int>(format.size()))) {
        BOOST_THROW_EXCEPTION(UnableToReadFormatMagicError());
    }
}

void writeFormatMagic(
    io::WriteBuffer &output,
    const boost::uuids::uuid &format)
{
    google::protobuf::io::CodedOutputStream outp(&output);
    outp.WriteRaw(&format, static_cast<int>(format.size()));
    if (outp.HadError()) {
        BOOST_THROW_EXCEPTION(UnableToWriteFormatMagicError());
    }
}

}
}
}
