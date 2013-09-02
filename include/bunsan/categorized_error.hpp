#pragma once

#include <bunsan/error.hpp>

#include <system_error>

namespace bunsan
{
    /// \warning Should not be inherited virtually.
    struct categorized_error: virtual error
    {
        categorized_error()=default;

        explicit categorized_error(const std::error_code &ec);
        categorized_error(const std::error_code &ec, const std::string &what);

        typedef boost::error_info<struct tag_error_code, std::error_code> error_code;
        typedef boost::error_info<struct tag_what_message, std::string> what_message;
    };
}

namespace boost
{
    std::string to_string(const bunsan::categorized_error::error_code &ec);
}
