#include <bunsan/binlogs/AbstractNamedLogWriter.hpp>

#include <boost/assert.hpp>
#include <boost/filesystem/operations.hpp>

namespace bunsan {
namespace binlogs {

void AbstractNamedLogWriter::reopen()
{
    reopen(path());
}

void AbstractNamedLogWriter::reopen(const boost::filesystem::path &newPath)
{
    close();
    open(newPath);
}

void AbstractNamedLogWriter::rotate(const boost::filesystem::path &renameTo)
{
    close();
    BOOST_ASSERT(state() == State::kEof);
    boost::filesystem::rename(path(), renameTo);
    open(path());
}

}
}
