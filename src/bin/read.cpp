#include "bunsan/binlogs/detail/make_unique.hpp"
#include "bunsan/binlogs/LogFactory.hpp"

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/gzip_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

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
        const boost::filesystem::path file = argv[i];
        boost::filesystem::ifstream fin(file, std::ios_base::binary);
        if (!fin.is_open()) {
            perror(file.c_str());
            return 3;
        }
        google::protobuf::io::IstreamInputStream input(&fin);
        google::protobuf::io::GzipInputStream gzip(&input);
        auto in = detail::make_unique<google::protobuf::io::CodedInputStream>(&gzip);

        std::string error;

        auto logReader = openReadOnly(std::move(in), &error);
        if (!logReader) {
            std::cerr << file << ": " << error << std::endl;
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
