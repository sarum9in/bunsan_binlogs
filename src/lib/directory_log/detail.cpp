#include <bunsan/binlogs/directory_log/detail.hpp>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

#include <algorithm>

namespace bunsan {
namespace binlogs {
namespace directory_log {
namespace detail {

constexpr std::size_t decpow(const std::size_t pow)
{
    return pow == 0 ? 1 : 10 * decpow(pow - 1);
}

namespace {

const std::string NAME_PREFIX = "bunsan_binlog_";
constexpr std::size_t ID_SIZE = 9;
constexpr std::size_t ID_END = decpow(ID_SIZE);
const std::string NAME_SUFFIX = ".gz";
const std::string NAME_FORMAT =
    NAME_PREFIX + "%|0" + boost::lexical_cast<std::string>(ID_SIZE) + "|" + NAME_SUFFIX;

template <typename F>
void walk(const boost::filesystem::path &path, const F &f)
{
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
                f(id, i->path());
            }
        }
    }
}

std::string toName(const std::size_t id)
{
    return str(boost::format(NAME_FORMAT) % id);
}

boost::optional<std::size_t> lastPathId(const boost::filesystem::path &path)
{
    boost::optional<std::size_t> maxId;
    walk(path,
        [&maxId](const std::size_t id, const boost::filesystem::path &/*path*/)
        {
            if (maxId) {
                maxId = std::max(id, *maxId);
            } else {
                maxId = id;
            }
        });
    return maxId;
}

}

std::vector<boost::filesystem::path> listDir(const boost::filesystem::path &path)
{
    std::vector<boost::filesystem::path> list;
    walk(path,
        [&list](const std::size_t /*id*/, const boost::filesystem::path &path)
        {
            list.push_back(path);
        });
    std::sort(list.begin(), list.end());
    return list;
}

boost::optional<boost::filesystem::path> lastPath(const boost::filesystem::path &path)
{
    const boost::optional<std::size_t> lastId = lastPathId(path);
    if (lastId) {
        return path / toName(*lastId);
    }
    return boost::none;
}

boost::filesystem::path nextPath(const boost::filesystem::path &path)
{
    const boost::optional<std::size_t> lastId = lastPathId(path);
    const std::size_t nextId = lastId ? *lastId + 1 : 0;
    if (nextId >= ID_END) {
        BOOST_THROW_EXCEPTION(TooManyLogFilesError());
    }
    return path / toName(nextId);
}

}
}
}
}
