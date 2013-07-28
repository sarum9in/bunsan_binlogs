#pragma once

#include "bunsan/binlogs/file/BaseBuffer.hpp"

#include "bunsan/binlogs/ReadBuffer.hpp"

#include <google/protobuf/io/zero_copy_stream_impl.h>

#include <boost/filesystem/path.hpp>

namespace bunsan {
namespace binlogs {
namespace file {

class ReadBuffer: public binlogs::ReadBuffer, public BaseBuffer {
public:
    /// \return false if !closed()
    bool open(const boost::filesystem::path &path);

    google::protobuf::io::ZeroCopyInputStream *istream() override;

    bool close() override;

    bool closed() const override;

    int getErrno() const override;

private:
    std::unique_ptr<google::protobuf::io::FileInputStream> istream_;
    int errno_ = 0;
};

}
}
}
