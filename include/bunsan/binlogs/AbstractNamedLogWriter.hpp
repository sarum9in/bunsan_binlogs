#pragma once

#include "bunsan/binlogs/NamedLogWriter.hpp"

namespace bunsan {
namespace binlogs {

class AbstractNamedLogWriter: public NamedLogWriter {
public:
    bool reopen(std::string *error=nullptr) override;
    bool reopen(const boost::filesystem::path &newPath, std::string *error=nullptr) override;
    bool rotate(const boost::filesystem::path &renameTo, std::string *error=nullptr) override;
};

}
}
