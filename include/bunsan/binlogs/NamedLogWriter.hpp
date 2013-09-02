#pragma once

#include <bunsan/binlogs/LogWriter.hpp>

#include <boost/filesystem/path.hpp>

namespace bunsan {
namespace binlogs {

class NamedLogWriter: public virtual LogWriter {
public:
    virtual boost::filesystem::path path() const=0;

    virtual void open(const boost::filesystem::path &path)=0;

    /// Append to already created log file. It must have the same format.
    virtual void append(const boost::filesystem::path &path)=0;

    /// reopen(path())
    virtual void reopen()=0;

    /// Close current sink and open new at newPath.
    virtual void reopen(const boost::filesystem::path &newPath)=0;

    /// Rename current sink to renameTo and reopen sink at path().
    virtual void rotate(const boost::filesystem::path &renameTo)=0;
};

}
}
