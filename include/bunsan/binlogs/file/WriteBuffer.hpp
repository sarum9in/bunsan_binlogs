#pragma once

#include "bunsan/binlogs/file/BaseBuffer.hpp"

#include "bunsan/binlogs/WriteBuffer.hpp"

#include <google/protobuf/io/zero_copy_stream_impl.h>

#include <boost/filesystem/path.hpp>

namespace bunsan {
namespace binlogs {
namespace file {

class WriteBuffer: public binlogs::WriteBuffer, public BaseBuffer {
public:
    /// \return false if !closed()
    bool open(const boost::filesystem::path &path);

    google::protobuf::io::ZeroCopyOutputStream *ostream() override;

    bool close() override;

    bool closed() const override;

    int getErrno() const override;

private:
    std::unique_ptr<google::protobuf::io::FileOutputStream> istream_;
    int errno_ = 0;
};

}
}
}
