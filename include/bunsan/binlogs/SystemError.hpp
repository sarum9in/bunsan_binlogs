#pragma once

#include <bunsan/binlogs/Error.hpp>

#include <bunsan/system_error.hpp>
#include <bunsan/filesystem/error.hpp>
#include <bunsan/forward_constructor.hpp>

#include <boost/uuid/uuid.hpp>

namespace bunsan {
namespace binlogs {

struct SystemError: bunsan::system_error, virtual Error {
    BUNSAN_INHERIT_CONSTRUCTOR(SystemError, system_error)
};

}
}
