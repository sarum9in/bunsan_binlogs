#include <bunsan/binlogs/LogFactory.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/format.hpp>

#include <iostream>

#include <cstdlib>

using namespace bunsan::binlogs;

int main(int argc, char *argv[])
{
    constexpr int FAST_CHECK_FAIL = 100;
    constexpr int RUN_FAIL = 1;

    if (argc == 1) {
        std::cerr << boost::format(R"EOF(Usage: %1% [log file]...

Runs fast check of log format.
If one of log files has invalid format or can't be opened returns %2%.
Returns %3% on any other error.
Information about log files is separated by at least two newlines.
If single file is specified no additional padding is added.
)EOF") % argv[0] % FAST_CHECK_FAIL % RUN_FAIL << std::flush;
        return 200;
    }

    bool fail = false;
    try {
        for (int i = 1; i < argc; ++i) {
            const boost::filesystem::path path = argv[i];
            if (i > 1) {
                std::cerr << '\n';;
            }
            try {
                auto logReader = openReadOnly(path);
                logReader->fastCheck();
                BOOST_ASSERT(logReader->eof());
                std::cerr << path << ": [OK]\n";
            } catch (std::exception &e) {
                fail = true;
                std::cerr << path << ": [FAIL]\n" << e.what() << "\n";
            }
        }
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return RUN_FAIL;
    }
    if (fail) {
        return FAST_CHECK_FAIL;
    }
}
