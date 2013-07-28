#pragma once

#include "bunsan/binlogs/BaseBuffer.hpp"

#include <boost/filesystem/path.hpp>

namespace bunsan {
namespace binlogs {
namespace file {

class BaseBuffer: public virtual binlogs::BaseBuffer {
public:
    bool error(std::string *error=nullptr) const override;

    /// \return 0 if no error occurred
    virtual int getErrno() const=0;
};

}
}
}
