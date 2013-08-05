#pragma once

#include <bunsan/binlogs/io/file/BasicBuffer.hpp>
#include <bunsan/binlogs/io/ReadBuffer.hpp>

#include <google/protobuf/io/zero_copy_stream_impl.h>

namespace bunsan {
namespace binlogs {
namespace io {
namespace file {

class ReadBuffer: public BasicBuffer<io::ReadBuffer, google::protobuf::io::FileInputStream> {
protected:
    std::unique_ptr<google::protobuf::io::FileInputStream> openFd(
        const boost::filesystem::path &path,
        int &errno_) override;
};

}
}
}
}
