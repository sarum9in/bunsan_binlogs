#pragma once

#include <bunsan/binlogs/io/ReadBuffer.hpp>
#include <bunsan/binlogs/io/WriteBuffer.hpp>

#include <memory>

namespace bunsan {
namespace binlogs {
namespace io {
namespace filter {
namespace gzip {

std::unique_ptr<ReadBuffer> open(std::unique_ptr<ReadBuffer> &&buffer, std::string *error=nullptr);

std::unique_ptr<WriteBuffer> open(std::unique_ptr<WriteBuffer> &&buffer, std::string *error=nullptr);

}
}
}
}
}
