#define BOOST_TEST_MODULE io
#include <boost/test/unit_test.hpp>

#include "bunsan/binlogs/io/file/open.hpp"
#include "bunsan/binlogs/io/filter/gzip.hpp"

#include "bunsan/testing/filesystem/read_data.hpp"
#include "bunsan/testing/filesystem/tempfiles.hpp"
#include "bunsan/testing/filesystem/write_data.hpp"

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/gzip_stream.h>

#include <boost/filesystem/operations.hpp>

using namespace bunsan::binlogs;

BOOST_AUTO_TEST_SUITE(io_)

struct FileFixture: private bunsan::testing::filesystem::tempfiles {
    FileFixture(): tmp(allocate()) {}

    boost::filesystem::path tmp;
};

BOOST_FIXTURE_TEST_SUITE(file_, FileFixture)

BOOST_AUTO_TEST_CASE(openRead)
{
    const std::string SOME_DATA = "Hello, world!";
    bunsan::testing::filesystem::write_data(tmp, SOME_DATA);
    std::string error;
    std::unique_ptr<io::ReadBuffer> buffer = io::file::openReadOnly(tmp, &error);
    BOOST_REQUIRE_MESSAGE(buffer, error);
    {
        google::protobuf::io::CodedInputStream is(buffer.get());
        std::string data;
        BOOST_REQUIRE(is.ReadString(&data, SOME_DATA.size()));
        BOOST_CHECK_EQUAL(data, SOME_DATA);
    }
    BOOST_CHECK(buffer->close());
    BOOST_CHECK(buffer->closed());
    BOOST_CHECK(!buffer->error());
}

BOOST_AUTO_TEST_CASE(openReadError)
{
    std::string error;
    std::unique_ptr<io::ReadBuffer> buffer = io::file::openReadOnly("/path/that/does/not/exist", &error);
    BOOST_CHECK(!buffer);
    BOOST_TEST_MESSAGE(error);
}

BOOST_AUTO_TEST_CASE(openWrite)
{
    const std::string SOME_DATA = "Hello, world!";
    std::string error;
    std::unique_ptr<io::WriteBuffer> buffer = io::file::openWriteOnly(tmp, &error);
    BOOST_REQUIRE_MESSAGE(buffer, error);
    {
        google::protobuf::io::CodedOutputStream os(buffer.get());
        os.WriteString(SOME_DATA);
        BOOST_REQUIRE(!os.HadError());
    }
    BOOST_REQUIRE(buffer->close());
    BOOST_CHECK(!buffer->error());
    BOOST_CHECK_EQUAL(bunsan::testing::filesystem::read_data(tmp), SOME_DATA);
}

BOOST_AUTO_TEST_CASE(openWriteError)
{
    bunsan::testing::filesystem::write_data(tmp, "");
    boost::filesystem::permissions(tmp, boost::filesystem::no_perms);
    std::string error;
    std::unique_ptr<io::WriteBuffer> buffer = io::file::openWriteOnly(tmp, &error);
    BOOST_CHECK(!buffer);
    BOOST_TEST_MESSAGE(error);
}

BOOST_AUTO_TEST_SUITE_END() // file_

BOOST_AUTO_TEST_SUITE(filter_)

BOOST_FIXTURE_TEST_SUITE(gzip_, FileFixture)

const std::string SOME_DATA = "Hello, world!";

void checkIsGzip(const boost::filesystem::path &path)
{
    const std::string data = bunsan::testing::filesystem::read_data(path);
    BOOST_REQUIRE(data.size() > 2);
    BOOST_REQUIRE(data[0] == char(0x1f));
    BOOST_REQUIRE(data[1] == char(0x8b));
}

std::string readGzip(const boost::filesystem::path &path)
{
    std::string error;
    std::string data;
    std::unique_ptr<io::ReadBuffer> buffer = io::file::openReadOnly(path, &error);
    BOOST_REQUIRE_MESSAGE(buffer, error);
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
    BOOST_REQUIRE(buffer->close());
    return data;
}

void writeGzip(const boost::filesystem::path &path, const std::string &data)
{
    std::string error;
    std::unique_ptr<io::WriteBuffer> buffer = io::file::openWriteOnly(path, &error);
    BOOST_REQUIRE_MESSAGE(buffer, error);
    {
        google::protobuf::io::GzipOutputStream gbuffer(buffer.get());
        {
            google::protobuf::io::CodedOutputStream os(&gbuffer);
            os.WriteString(data);
            BOOST_REQUIRE(!os.HadError());
        }
    }
    BOOST_REQUIRE(buffer->close());
    checkIsGzip(path);
}

BOOST_AUTO_TEST_CASE(openRead)
{
    writeGzip(tmp, SOME_DATA);
    std::string error;
    std::unique_ptr<io::ReadBuffer> buffer = io::file::openReadOnly(tmp, &error);
    BOOST_REQUIRE_MESSAGE(buffer, error);
    buffer = io::filter::gzip::open(std::move(buffer), &error);
    BOOST_REQUIRE_MESSAGE(buffer, error);
    {
        google::protobuf::io::CodedInputStream is(buffer.get());
        std::string data;
        BOOST_REQUIRE(is.ReadString(&data, SOME_DATA.size()));
        BOOST_CHECK_EQUAL(data, SOME_DATA);
    }
    BOOST_CHECK(buffer->close());
    BOOST_CHECK(buffer->closed());
    BOOST_CHECK(!buffer->error());
}

BOOST_AUTO_TEST_CASE(openReadError)
{
    bunsan::testing::filesystem::write_data(tmp, "NOT GZIP DATA");
    std::string error;
    std::unique_ptr<io::ReadBuffer> buffer = io::file::openReadOnly(tmp, &error);
    BOOST_REQUIRE_MESSAGE(buffer, error);
    buffer = io::filter::gzip::open(std::move(buffer), &error);
    if (buffer) {
        google::protobuf::io::CodedInputStream is(buffer.get());
        std::string data;
        BOOST_REQUIRE(!is.ReadString(&data, 10));
        BOOST_REQUIRE(buffer->error(&error));
    }
    BOOST_TEST_MESSAGE("Passed error test: " << tmp << ": " << error);
}

BOOST_AUTO_TEST_CASE(openWrite)
{
    std::string error;
    std::unique_ptr<io::WriteBuffer> buffer = io::filter::gzip::open(io::file::openWriteOnly(tmp, &error));
    BOOST_REQUIRE_MESSAGE(buffer, error);
    {
        google::protobuf::io::CodedOutputStream os(buffer.get());
        os.WriteString(SOME_DATA);
        BOOST_REQUIRE(!os.HadError());
    }
    BOOST_REQUIRE(buffer->close());
    BOOST_CHECK(!buffer->error());
    checkIsGzip(tmp);
    BOOST_CHECK_EQUAL(readGzip(tmp), SOME_DATA);
}

BOOST_AUTO_TEST_SUITE_END() // gzip_

BOOST_AUTO_TEST_SUITE_END() // filter_

BOOST_AUTO_TEST_SUITE_END() // io_
