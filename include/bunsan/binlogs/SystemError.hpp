#pragma once

#include <bunsan/binlogs/Error.hpp>

#include <bunsan/system_error.hpp>
#include <bunsan/filesystem/error.hpp>

#include <boost/uuid/uuid.hpp>

namespace bunsan {
namespace binlogs {

struct SystemError: bunsan::system_error, virtual Error {
    using bunsan::system_error::system_error;
};

}
}
