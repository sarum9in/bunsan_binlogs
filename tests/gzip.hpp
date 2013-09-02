#pragma once

#include <bunsan/binlogs/io/file/open.hpp>

#include <bunsan/testing/filesystem/read_data.hpp>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/gzip_stream.h>

void checkIsGzip(const boost::filesystem::path &path)
{
    const std::string data = bunsan::testing::filesystem::read_data(path);
    BOOST_REQUIRE(data.size() > 2);
    BOOST_REQUIRE(data[0] == char(0x1f));
    BOOST_REQUIRE(data[1] == char(0x8b));
}

std::string readGzip(const boost::filesystem::path &path)
{
    std::string data;
    std::unique_ptr<bunsan::binlogs::io::ReadBuffer> buffer =
        bunsan::binlogs::io::file::openReadOnly(path);
    {
        google::protobuf::io::GzipInputStream gbuffer(buffer.get());
        const void *chunk;
        int size;
        while (gbuffer.Next(&chunk, &size)) {
            data.append(static_cast<const char *>(chunk), size);
        }
        const char *const err = gbuffer.ZlibErrorMessage();
        if (err) {
            BOOST_FAIL(err);
        }
    }
    buffer->close();
    return data;
}

void writeGzip(const boost::filesystem::path &path, const std::string &data)
{
    std::unique_ptr<bunsan::binlogs::io::WriteBuffer> buffer =
        bunsan::binlogs::io::file::openWriteOnly(path);
    {
        google::protobuf::io::GzipOutputStream gbuffer(buffer.get());
        {
            google::protobuf::io::CodedOutputStream os(&gbuffer);
            os.WriteString(data);
            BOOST_REQUIRE(!os.HadError());
        }
    }
    buffer->close();
    checkIsGzip(path);
}
