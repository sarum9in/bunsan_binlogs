#pragma once

#include <bunsan/binlogs/MessageType.hpp>

#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

namespace bunsan {
namespace binlogs {
namespace v1 {

class MessageType: public binlogs::MessageType {
public:
    MessageType(const google::protobuf::Descriptor *const descriptor,
                const google::protobuf::Message *const prototype);

    std::string typeName() const override;

    const google::protobuf::Message *prototype() const override;

private:
    const google::protobuf::Descriptor *const descriptor_;
    const google::protobuf::Message *const prototype_;
};

}
}
}
