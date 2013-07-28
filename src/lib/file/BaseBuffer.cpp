#include "bunsan/binlogs/file/BaseBuffer.hpp"

#include <system_error>

namespace bunsan {
namespace binlogs {
namespace file {

bool BaseBuffer::error(std::string *error) const
{
    if (closed()) {
        return false;
    }
    if (getErrno()) {
        if (error) {
            *error = std::system_category().message(getErrno());
        }
        return true;
    }
    return false;
}

}
}
}
