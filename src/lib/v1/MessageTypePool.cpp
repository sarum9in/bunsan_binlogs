#include "bunsan/binlogs/v1/MessageTypePool.hpp"
#include "bunsan/binlogs/detail/make_unique.hpp"

#include <boost/assert.hpp>
#include <boost/format.hpp>
#include <boost/iterator/transform_iterator.hpp>

#include <functional>

namespace bunsan {
namespace binlogs {
namespace v1 {

MessageTypePool::MessageTypePool():
    pool_(&db_) {}

bool MessageTypePool::Init(const Header &header, std::string *error)
{
    for (const google::protobuf::FileDescriptorProto &proto: header.proto.file()) {
        if (!db_.Add(proto)) {
            if (error) {
                *error = "File already in db.";
            }
            return false;
        }
    }

    messageTypes_.resize(header.types.size());
    for (std::size_t i = 0; i < header.types.size(); ++i) {
        const std::string &type = header.types[i];
        std::unique_ptr<MessageType> &messageType = messageTypes_[i];

        const google::protobuf::Descriptor *const descriptor =
            pool_.FindMessageTypeByName(type);
        if (!descriptor) {
            if (error) {
                *error = str(boost::format("Unable to allocate descriptor of \"%1%\" type.") % type);
            }
            return false;
        }
        const google::protobuf::Message *const prototype = factory_.GetPrototype(descriptor);
        if (!prototype) {
            if (error) {
                *error = str(boost::format("Unable to allocate prototype of \"%1%\" type.") % type);
            }
            return false;
        }
        messageType = detail::make_unique<MessageType>(descriptor, prototype);
    }
    return true;
}

Header MessageTypePool::header() const
{
    return header_;
}

MessageTypePool::Id MessageTypePool::typeId(const std::string &typeName) const
{
    for (Id id = 0; id < messageTypes_.size(); ++id) {
        if (messageTypes_[id]->typeName() == typeName) {
            return id;
        }
    }
    return npos;
}

const MessageType *MessageTypePool::type(const Id id) const
{
    if (id < messageTypes_.size()) {
        return messageTypes_[id].get();
    } else {
        return nullptr;
    }
}

const MessageType *MessageTypePool::type(const std::string &typeName) const
{
    const Id id = typeId(typeName);
    if (id == npos) {
        return nullptr;
    } else {
        return type(id);
    }
}

namespace {

struct PtrGet: std::unary_function<const std::unique_ptr<MessageType> &, const MessageType *> {
    const MessageType *operator()(const std::unique_ptr<MessageType> &ptr) const
    {
        return ptr.get();
    }
};

}

MessageTypePool::range MessageTypePool::types() const
{
    const auto begin = boost::make_transform_iterator<PtrGet>(messageTypes_.begin());
    const auto end = boost::make_transform_iterator<PtrGet>(messageTypes_.end());
    return range(begin, end);
}

}
}
}
