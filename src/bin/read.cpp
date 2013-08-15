#include <bunsan/binlogs/LogFactory.hpp>

#include <google/protobuf/descriptor.h>

#include <boost/filesystem/path.hpp>

#include <iostream>

using namespace bunsan::binlogs;

int main(int argc, char *argv[])
{
    for (int i = 1; i < argc; ++i) {
        const boost::filesystem::path path = argv[i];
        std::string error;

        auto logReader = openReadOnly(path, &error);
        if (!logReader) {
            std::cerr << error << std::endl;
            return 7;
        }

        while (logReader->usable()) {
            const auto message = logReader->read(&error);
            if (message) {
                std::cout << message->GetDescriptor()->full_name() << '\n' <<
                             message->DebugString() << '\n';
            } else {
                if (!logReader->eof()) {
                    std::cerr << error << std::endl;
                    if (!logReader->usable()) {
                        return 4;
                    }
                }
            }
        }
        if (!logReader->eof()) {
            return 1;
        }
    }
}
