#include <bunsan/binlogs/BaseIo.hpp>

namespace bunsan {
namespace binlogs {

BaseIo::~BaseIo() {}

bool BaseIo::usable() const
{
    return ok() || fail();
}

bool BaseIo::ok() const
{
    return state() == State::kOk;
}

bool BaseIo::eof() const
{
    return state() == State::kEof;
}

bool BaseIo::fail() const
{
    return state() == State::kFail;
}

bool BaseIo::bad() const
{
    return state() == State::kBad;
}

}
}
