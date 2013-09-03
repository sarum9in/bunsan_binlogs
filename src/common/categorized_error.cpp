#include <bunsan/categorized_error.hpp>

#include <sstream>

namespace bunsan
{
    categorized_error::categorized_error(const std::error_code &ec)
    {
        (*this) << error_code(ec);
    }

    categorized_error::categorized_error(const std::error_code &ec, const std::string &what):
        categorized_error(ec)
    {
        (*this) << what_message(what);
    }
}

namespace boost
{
    std::string to_string(const bunsan::categorized_error::error_code &ec)
    {
        std::ostringstream sout;
        sout << "[" << bunsan::error::info_name(ec) << "] = " <<
                ec.value() << ": " << ec.value().message() << '\n';
        return sout.str();
    }
}
