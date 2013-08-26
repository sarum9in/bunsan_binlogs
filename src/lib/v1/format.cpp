#include <bunsan/binlogs/v1/format.hpp>

#include <boost/uuid/string_generator.hpp>

namespace bunsan {
namespace binlogs {
namespace v1 {

const std::string NAME = "v1";
const boost::uuids::uuid MAGIC_FORMAT = boost::uuids::string_generator()("f6a03dc0-eaf5-11e2-91e2-0800200c9a66");
const boost::uuids::uuid MAGIC_FOOTER = boost::uuids::string_generator()("710d7bbf-f8ba-4b26-83f7-805b64c316e6");

}
}
}
