#pragma once

#include <boost/noncopyable.hpp>

#include <iostream>
#include <string>

namespace bunsan {
namespace binlogs {

class BaseIo: private boost::noncopyable
{
public:
    enum class State {
        kOk,
        kEof,
        kFail,
        kBad,
    };

    virtual ~BaseIo();

    virtual bool close(std::string *error=nullptr)=0;

    virtual State state() const=0;

    bool usable() const;
    bool ok() const;
    bool bad() const;
    bool eof() const;
    bool fail() const;
};

inline std::ostream &operator<<(std::ostream &out, const BaseIo::State state)
{
    switch (state) {
    case BaseIo::State::kOk:
        return out << "OK";
    case BaseIo::State::kEof:
        return out << "EOF";
    case BaseIo::State::kFail:
        return out << "FAIL";
    case BaseIo::State::kBad:
        return out << "BAD";
    default:
        return out << "UNKNOWN" << static_cast<int>(state);
    }
}

}
}
