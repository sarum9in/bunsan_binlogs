#pragma once

#include "bunsan/binlogs/file/BasicBuffer.hpp"

#include "bunsan/binlogs/WriteBuffer.hpp"

#include <google/protobuf/io/zero_copy_stream_impl.h>

namespace bunsan {
namespace binlogs {
namespace file {

class WriteBuffer: public BasicBuffer<binlogs::WriteBuffer, google::protobuf::io::FileOutputStream> {
public:
    google::protobuf::io::ZeroCopyOutputStream *ostream() override;

protected:
    std::unique_ptr<google::protobuf::io::FileOutputStream> openFd(
        const boost::filesystem::path &path,
        int &errno_) override;
};

}
}
}
