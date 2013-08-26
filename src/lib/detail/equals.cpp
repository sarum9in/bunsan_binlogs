#include <bunsan/binlogs/detail/equals.hpp>

namespace bunsan {
namespace binlogs {
namespace detail {

bool equals(const google::protobuf::Message &lhs,
            const google::protobuf::Message &rhs)
{
    std::string ls, rs;
    if (!lhs.SerializePartialToString(&ls))
        return false;
    if (!rhs.SerializePartialToString(&rs))
        return false;
    return ls == rs;
}

}
}
}
