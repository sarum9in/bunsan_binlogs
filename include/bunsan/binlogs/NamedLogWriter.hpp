#pragma once

#include <bunsan/binlogs/LogWriter.hpp>

#include <boost/filesystem/path.hpp>

namespace bunsan {
namespace binlogs {

class NamedLogWriter: public virtual LogWriter {
public:
    virtual boost::filesystem::path path() const=0;

    virtual bool open(const boost::filesystem::path &path, std::string *error=nullptr)=0;

    /// Append to already created log file. It must have the same format.
    virtual bool append(const boost::filesystem::path &path, std::string *error=nullptr)=0;

    /// reopen(path())
    virtual bool reopen(std::string *error=nullptr)=0;

    /// Close current sink and open new at newPath.
    virtual bool reopen(const boost::filesystem::path &newPath, std::string *error=nullptr)=0;

    /// Rename current sink to renameTo and reopen sink at path().
    virtual bool rotate(const boost::filesystem::path &renameTo, std::string *error=nullptr)=0;
};

}
}
