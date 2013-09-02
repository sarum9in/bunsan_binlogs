#pragma once

#include <bunsan/categorized_error.hpp>
#include <bunsan/forward_constructor.hpp>

namespace bunsan
{
    struct system_error: categorized_error
    {
        system_error();
        explicit system_error(const int errcode);
        explicit system_error(const std::string &what);
        system_error(const int errcode, const std::string &what);

        BUNSAN_INHERIT_CONSTRUCTOR(system_error, categorized_error)
    };
}
