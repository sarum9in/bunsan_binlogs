#include <bunsan/binlogs/detail/directory_log.hpp>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

namespace bunsan {
namespace binlogs {
namespace detail {
namespace directory_log {

boost::filesystem::path nextPath(const boost::filesystem::path &path)
{
    static const std::string NAME_PREFIX = "bunsan_binlog_";
    constexpr std::size_t ID_SIZE = 9;
    static const std::string NAME_SUFFIX = ".gz";
    static const std::string NAME_FORMAT =
        NAME_PREFIX + "%|0" + boost::lexical_cast<std::string>(ID_SIZE) + "|" + NAME_SUFFIX;
    std::size_t maxId = 0;
    bool found = false;
    for (boost::filesystem::directory_iterator i(path), end; i != end; ++i) {
        const boost::filesystem::path filename = i->path().filename();
        const std::string name = filename.string();
        if (name.size() == NAME_PREFIX.size() + ID_SIZE + NAME_SUFFIX.size() &&
            boost::algorithm::starts_with(name, NAME_PREFIX) &&
            boost::algorithm::ends_with(name, NAME_SUFFIX))
        {
            const std::string nameId =
                name.substr(NAME_PREFIX.size(), ID_SIZE);
            if (boost::algorithm::all(nameId, boost::algorithm::is_digit())) {
                const std::size_t id = boost::lexical_cast<std::size_t>(nameId);
                maxId = std::max(id, maxId);
                found = true;
            }
        }
    }
    return path / str(boost::format(NAME_FORMAT) % (maxId + found));
}

}
}
}
}
