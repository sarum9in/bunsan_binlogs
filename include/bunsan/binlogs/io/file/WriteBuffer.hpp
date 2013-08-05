#pragma once

#include <bunsan/binlogs/io/file/BasicBuffer.hpp>
#include <bunsan/binlogs/io/WriteBuffer.hpp>

#include <google/protobuf/io/zero_copy_stream_impl.h>

namespace bunsan {
namespace binlogs {
namespace io {
namespace file {

class WriteBuffer: public BasicBuffer<io::WriteBuffer, google::protobuf::io::FileOutputStream> {
protected:
    std::unique_ptr<google::protobuf::io::FileOutputStream> openFd(
        const boost::filesystem::path &path,
        int &errno_) override;
};

}
}
}
}
