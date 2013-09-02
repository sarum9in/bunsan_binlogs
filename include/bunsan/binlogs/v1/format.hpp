#pragma once

#include <boost/uuid/uuid.hpp>

namespace bunsan {
namespace binlogs {
namespace v1 {

extern const std::string NAME;
extern const boost::uuids::uuid MAGIC_FORMAT;
extern const boost::uuids::uuid MAGIC_FOOTER;
extern const boost::uuids::uuid MAGIC_CONTINUE;

}
}
}
