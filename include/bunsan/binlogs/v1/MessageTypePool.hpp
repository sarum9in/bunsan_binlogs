#pragma once

#include "bunsan/binlogs/v1/MessageType.hpp"

#include "bunsan/binlogs/Header.hpp"
#include "bunsan/binlogs/MessageTypePool.hpp"

#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor_database.h>
#include <google/protobuf/dynamic_message.h>

#include <limits>

#include <cstdint>

namespace bunsan {
namespace binlogs {
namespace v1 {

class MessageTypePool: public binlogs::MessageTypePool {
public:
    typedef std::uint32_t Id;
    static constexpr Id npos = std::numeric_limits<Id>::max();

    MessageTypePool();

    bool Init(const Header &header, std::string *error=nullptr);

    /// \return npos if not found
    Id typeId(const std::string &typeName) const;

    /*!
     * \pre id is valid
     * \post return value is not nullptr
     */
    const MessageType *type(const Id id) const;

    /// \return nullptr if not found
    const MessageType *type(const std::string &typeName) const override;

    range types() const override;

private:
    google::protobuf::SimpleDescriptorDatabase db_;
    google::protobuf::DescriptorPool pool_;
    google::protobuf::DynamicMessageFactory factory_;
    Header header_;
    std::vector<std::unique_ptr<MessageType>> messageTypes_;
};

}
}
}
