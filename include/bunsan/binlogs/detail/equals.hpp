#pragma once

#include <google/protobuf/message.h>

namespace bunsan {
namespace binlogs {
namespace detail {

bool equals(const google::protobuf::Message &lhs,
            const google::protobuf::Message &rhs);

}
}
}
