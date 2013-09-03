#include <bunsan/binlogs/Error.hpp>

#include <boost/uuid/uuid_io.hpp>

namespace boost {

std::string to_string(const bunsan::binlogs::FormatError::Format &format)
{
    std::ostringstream sout;
    sout << '[' << bunsan::error::info_name(format) << "] = " <<
            '{' << format.value() << "}\n";
    return sout.str();
}

std::string to_string(const bunsan::binlogs::UninitializedMessageError::UninitializedFields &fields)
{
    std::ostringstream sout;
    sout << '[' << bunsan::error::info_name(fields) << "] = ";
    bool first = true;
    for (const std::string &field: fields.value()) {
        if (!first) {
            sout << ", ";
        }
        sout << field;
        first = false;
    }
    sout << '\n';
    return sout.str();
}

}
