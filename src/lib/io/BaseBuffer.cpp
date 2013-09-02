#include <bunsan/binlogs/io/BaseBuffer.hpp>
#include <bunsan/binlogs/io/Error.hpp>

namespace bunsan {
namespace binlogs {
namespace io {

BaseBuffer::~BaseBuffer() {}

void BaseBuffer::checkError() const
{
    std::string error_;
    if (error(&error_)) {
        BOOST_THROW_EXCEPTION(Error() << Error::Message(error_));
    }
}

}
}
}
