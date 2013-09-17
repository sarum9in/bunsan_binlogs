#include <bunsan/binlogs/LogFactory.hpp>

#include <google/protobuf/descriptor.h>

#include <boost/filesystem/path.hpp>
#include <boost/format.hpp>

#include <iostream>

using namespace bunsan::binlogs;

int main(int argc, char *argv[])
{
    if (argc == 1) {
        std::cerr << boost::format(R"EOF(Usage: %1% [log file]...

Produces readable text representation of each binary log file
generated by bunsan::binlogs library.
)EOF") % argv[0] << std::flush;
        return 200;
    }

    try {
        for (int i = 1; i < argc; ++i) {
            const boost::filesystem::path path = argv[i];
            auto logReader = openReadOnly(path);

            while (logReader->usable()) {
                try {
                    const auto message = logReader->read();
                    if (message) {
                        std::cout << message->GetDescriptor()->full_name() << '\n' <<
                                     message->DebugString() << '\n';
                    } else {
                        BOOST_ASSERT(logReader->eof());
                        break;
                    }
                } catch (std::exception &e) {
                    if (logReader->usable()) {
                        std::cerr << e.what() << std::endl;
                    } else {
                        throw;
                    }
                }
            }
            BOOST_ASSERT(logReader->eof());
        }
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
