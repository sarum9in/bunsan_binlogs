#pragma once

#include "bunsan/binlogs/file/BasicBuffer.hpp"

#include "bunsan/binlogs/ReadBuffer.hpp"

#include <google/protobuf/io/zero_copy_stream_impl.h>

namespace bunsan {
namespace binlogs {
namespace file {

class ReadBuffer: public BasicBuffer<binlogs::ReadBuffer, google::protobuf::io::FileInputStream> {
public:
    google::protobuf::io::ZeroCopyInputStream *istream() override;

protected:
    std::unique_ptr<google::protobuf::io::FileInputStream> openFd(
        const boost::filesystem::path &path,
        int &errno_) override;
};

}
}
}
