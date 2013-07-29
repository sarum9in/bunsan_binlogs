#include "bunsan/binlogs/io/filter/gzip_if.hpp"
#include "bunsan/binlogs/io/filter/gzip.hpp"

#include "bunsan/binlogs/detail/make_unique.hpp"

#include <boost/assert.hpp>

#include <cstdint>

namespace bunsan {
namespace binlogs {
namespace io {
namespace filter {
namespace gzip {

std::unique_ptr<ReadBuffer> openIfGzip(std::unique_ptr<ReadBuffer> &&buffer, std::string *error)
{
    const void *data;
    int size;
    if (buffer->istream()->Next(&data, &size)) {
        BOOST_ASSERT(size >= 2); // TODO implement other case
        const std::uint8_t *const s = static_cast<const std::uint8_t *>(data);
        const std::uint8_t magic[] = {s[0], s[1]};
        buffer->istream()->BackUp(size);

        if (magic[0] == 0x1f && magic[1] == 0x8b) {
            return open(std::move(buffer), error);
        } else {
            return std::move(buffer);
        }
    } else {
        BOOST_VERIFY(buffer->error(error));
        return std::move(buffer);
    }
}

}
}
}
}
}
