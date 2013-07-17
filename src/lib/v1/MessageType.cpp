#include "bunsan/binlogs/v1/MessageType.hpp"

#include <boost/assert.hpp>

namespace bunsan {
namespace binlogs {
namespace v1 {

MessageType::MessageType(const google::protobuf::Descriptor *const descriptor,
                         const google::protobuf::Message *const prototype):
    descriptor_(descriptor), prototype_(prototype)
{
    BOOST_ASSERT(descriptor_);
    BOOST_ASSERT(prototype_);
}

std::string MessageType::typeName() const
{
    return descriptor_->full_name();
}

const google::protobuf::Message *MessageType::prototype() const
{
    return prototype_;
}

}
}
}
