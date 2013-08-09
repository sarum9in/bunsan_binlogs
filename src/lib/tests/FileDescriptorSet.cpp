#include <bunsan/binlogs/tests/FileDescriptorSet.hpp>
#include <bunsan/binlogs/tests/Message1.pb.h>
#include <bunsan/binlogs/tests/Message2.pb.h>

#include <google/protobuf/descriptor.h>

#include <mutex>

namespace bunsan {
namespace binlogs {
namespace tests {

static google::protobuf::FileDescriptorSet fdSet;

static void Init()
{
    Message1::descriptor()->file()->CopyTo(fdSet.add_file());
    Message2::descriptor()->file()->CopyTo(fdSet.add_file());
}

const google::protobuf::FileDescriptorSet &fileDescriptorSet()
{
    static std::once_flag flag;
    std::call_once(flag, Init);
    return fdSet;
}

}
}
}
