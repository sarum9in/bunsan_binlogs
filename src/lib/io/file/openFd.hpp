#pragma once

#include "bunsan/binlogs/detail/make_unique.hpp"

#include <boost/filesystem/path.hpp>
#include <boost/scope_exit.hpp>

#include <fcntl.h>

namespace bunsan {
namespace binlogs {
namespace io {
namespace file {

template <typename Stream>
std::unique_ptr<Stream> openFd(const boost::filesystem::path &path, const int flags, int &errno_)
{
    std::unique_ptr<Stream> stream;
    errno_ = 0;

    int fd = 0;
    BOOST_SCOPE_EXIT_ALL(&fd)
    {
        if (fd) {
            ::close(fd);
        }
    };

    fd = ::open(path.c_str(), flags, 0666);
    if (fd < 0) {
        errno_ = errno;
        return false;
    }
    stream = detail::make_unique<Stream>(fd);
    stream->SetCloseOnDelete(true);
    fd = 0; // disable scope exit

    return std::move(stream);
}

}
}
}
}
