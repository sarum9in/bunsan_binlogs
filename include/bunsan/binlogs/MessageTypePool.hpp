#pragma once

#include "bunsan/binlogs/MessageType.hpp"

#include <boost/range/any_range.hpp>

#include <string>

namespace bunsan {
namespace binlogs {

class MessageTypePool {
public:
    typedef boost::any_range<const MessageType *,
                             boost::forward_traversal_tag,
                             const MessageType *,
                             std::ptrdiff_t> range;
    typedef range::const_iterator const_iterator;
    typedef range::iterator iterator;

    virtual ~MessageTypePool();

    virtual const MessageType *type(const std::string &typeName) const=0;

    virtual range types() const=0;
};

inline MessageTypePool::const_iterator begin(const MessageTypePool &messageTypePool)
{
    return messageTypePool.types().begin();
}

inline MessageTypePool::const_iterator end(const MessageTypePool &messageTypePool)
{
    return messageTypePool.types().end();
}

}
}
