#pragma once

#include <bunsan/binlogs/io/ReadBuffer.hpp>
#include <bunsan/binlogs/io/WriteBuffer.hpp>

#include <boost/uuid/uuid.hpp>

namespace bunsan {
namespace binlogs {
namespace detail {

void readFormatMagic(
    io::ReadBuffer &input,
    boost::uuids::uuid &format);

void writeFormatMagic(
    io::WriteBuffer &output,
    const boost::uuids::uuid &format);

}
}
}
