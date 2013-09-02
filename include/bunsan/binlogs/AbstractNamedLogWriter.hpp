#pragma once

#include <bunsan/binlogs/NamedLogWriter.hpp>

namespace bunsan {
namespace binlogs {

class AbstractNamedLogWriter: public NamedLogWriter {
public:
    void reopen() override;
    void reopen(const boost::filesystem::path &newPath) override;
    void rotate(const boost::filesystem::path &renameTo) override;
};

}
}
