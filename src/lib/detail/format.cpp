#include <bunsan/binlogs/detail/format.hpp>

#include <google/protobuf/io/coded_stream.h>

namespace bunsan {
namespace binlogs {
namespace detail {

bool readFormatMagic(
    io::ReadBuffer &input,
    boost::uuids::uuid &format,
    std::string *error)
{
    google::protobuf::io::CodedInputStream inp(&input);
    if (!inp.ReadRaw(&format, static_cast<int>(format.size()))) {
        if (error) {
            *error = "Unable to read format magic.";
        }
        return false;
    }
    return true;
}

bool writeFormatMagic(
    io::WriteBuffer &output,
    const boost::uuids::uuid &format,
    std::string *error)
{
    google::protobuf::io::CodedOutputStream outp(&output);
    outp.WriteRaw(&format, static_cast<int>(format.size()));
    if (outp.HadError()) {
        if (error) {
            *error = "Unable to write format magic.";
        }
        return false;
    }
    return true;
}

}
}
}
