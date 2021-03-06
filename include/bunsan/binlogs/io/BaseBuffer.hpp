#pragma once

#include <boost/noncopyable.hpp>

#include <string>

namespace bunsan {
namespace binlogs {
namespace io {

class BaseBuffer: private boost::noncopyable {
public:
    virtual ~BaseBuffer();

    virtual void close()=0;

    virtual bool closed() const=0;

    virtual bool error(std::string *error=nullptr) const=0;

    /// \throws if (error())
    virtual void checkError() const;
};

}
}
}
