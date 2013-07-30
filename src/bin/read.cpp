#include "bunsan/binlogs/detail/make_unique.hpp"
#include "bunsan/binlogs/LogFactory.hpp"
#include "bunsan/binlogs/io/file/open.hpp"
#include "bunsan/binlogs/io/filter/gzip.hpp"

#include <google/protobuf/descriptor.h>

#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/path.hpp>

#include <fstream>
#include <iostream>

#include <cerrno>
#include <cstdio>

using namespace bunsan::binlogs;

int main(int argc, char *argv[])
{
    for (int i = 1; i < argc; ++i) {
        const boost::filesystem::path path = argv[i];
        std::string error;

        std::unique_ptr<io::ReadBuffer> buffer = io::file::openReadOnly(path, &error);
        if (!buffer) {
            std::cerr << error << std::endl;
            return 5;
        }
        buffer = io::filter::gzip::open(std::move(buffer), &error);
        if (!buffer) {
            std::cerr << error << std::endl;
            return 6;
        }

        auto logReader = openReadOnly(std::move(buffer), &error);
        if (!logReader) {
            std::cerr << path << ": " << error << std::endl;
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
