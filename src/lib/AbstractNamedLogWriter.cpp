#include "bunsan/binlogs/AbstractNamedLogWriter.hpp"

#include <boost/assert.hpp>
#include <boost/filesystem/operations.hpp>

namespace bunsan {
namespace binlogs {

bool AbstractNamedLogWriter::reopen(std::string *error)
{
    return reopen(path(), error);
}

bool AbstractNamedLogWriter::reopen(const boost::filesystem::path &newPath, std::string *error)
{
    if (!close(error)) {
        return false;
    }
    return open(newPath, error);
}

bool AbstractNamedLogWriter::rotate(const boost::filesystem::path &renameTo, std::string *error)
{
    if (!close(error)) {
        return false;
    }
    BOOST_ASSERT(state() == State::kEof);
    {
        boost::system::error_code ec;
        boost::filesystem::rename(path(), renameTo, ec);
        if (ec) {
            if (error) {
                *error = ec.message();
            }
            return false;
        }
    }
    return open(path(), error);
}

}
}
