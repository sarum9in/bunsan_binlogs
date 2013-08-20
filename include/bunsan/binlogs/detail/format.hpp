#pragma once

#include <bunsan/binlogs/io/ReadBuffer.hpp>
#include <bunsan/binlogs/io/WriteBuffer.hpp>

#include <boost/uuid/uuid.hpp>

namespace bunsan {
namespace binlogs {
namespace detail {

bool readFormatMagic(
    io::ReadBuffer &input,
    boost::uuids::uuid &format,
    std::string *error=nullptr);

bool writeFormatMagic(
    io::WriteBuffer &output,
    const boost::uuids::uuid &format,
    std::string *error=nullptr);

}
}
}
