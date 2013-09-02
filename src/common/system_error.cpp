#include <bunsan/system_error.hpp>

namespace bunsan
{
    system_error::system_error():
        system_error(errno) {}

    system_error::system_error(const std::string &what):
        system_error()
    {
        (*this) << what_message(what);
    }

    system_error::system_error(const int errcode):
        system_error(std::error_code(errcode, std::system_category())) {}

    system_error::system_error(const int errcode, const std::string &what):
        system_error(std::error_code(errcode, std::system_category()), what) {}
}
