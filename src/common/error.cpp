#include <bunsan/error.hpp>

bunsan::error::error() {}

bunsan::error::error(const std::string &message_)
{
    (*this) << message(message_);
}

const char *bunsan::error::what() const noexcept
{
    return boost::diagnostic_information_what(*this);
}

bunsan::error &bunsan::error::enable_nested(const boost::exception_ptr &ptr)
{
    (*this) << nested_exception(ptr);
    return *this;
}

bunsan::error &bunsan::error::enable_nested_current()
{
    return enable_nested(boost::current_exception());
}
