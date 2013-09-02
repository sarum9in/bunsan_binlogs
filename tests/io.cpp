#define BOOST_TEST_MODULE io
#include <boost/test/unit_test.hpp>

#include "gzip.hpp"

#include <bunsan/binlogs/io/file/open.hpp>
#include <bunsan/binlogs/io/filter/gzip.hpp>

#include <bunsan/testing/filesystem/read_data.hpp>
#include <bunsan/testing/filesystem/tempfile.hpp>
#include <bunsan/testing/filesystem/write_data.hpp>

#include <google/protobuf/io/coded_stream.h>

#include <boost/filesystem/operations.hpp>

using namespace bunsan::binlogs;

BOOST_AUTO_TEST_SUITE(io_)

BOOST_FIXTURE_TEST_SUITE(file_, bunsan::testing::filesystem::tempfile)

BOOST_AUTO_TEST_CASE(openAppend)
{
    const std::string PREFIX_DATA = "Hello, ";
    const std::string POSTFIX_DATA = "world!";
    bunsan::testing::filesystem::write_data(path, PREFIX_DATA);
    std::unique_ptr<io::WriteBuffer> buffer = io::file::openAppendOnly(path);
    {
        google::protobuf::io::CodedOutputStream os(buffer.get());
        os.WriteString(POSTFIX_DATA);
        BOOST_REQUIRE(!os.HadError());
    }
    buffer->close();
    BOOST_CHECK(!buffer->error());
    BOOST_CHECK_EQUAL(bunsan::testing::filesystem::read_data(path), PREFIX_DATA + POSTFIX_DATA);
}

BOOST_AUTO_TEST_CASE(openAppendError)
{
    BOOST_CHECK_THROW(io::file::openReadOnly("/path/that/does/not/exist"), std::exception);
}

BOOST_AUTO_TEST_CASE(openRead)
{
    const std::string SOME_DATA = "Hello, world!";
    bunsan::testing::filesystem::write_data(path, SOME_DATA);
    std::unique_ptr<io::ReadBuffer> buffer = io::file::openReadOnly(path);
    {
        google::protobuf::io::CodedInputStream is(buffer.get());
        std::string data;
        BOOST_REQUIRE(is.ReadString(&data, SOME_DATA.size()));
        BOOST_CHECK_EQUAL(data, SOME_DATA);
    }
    buffer->close();
    BOOST_CHECK(buffer->closed());
    BOOST_CHECK(!buffer->error());
}

BOOST_AUTO_TEST_CASE(openReadError)
{
    BOOST_CHECK_THROW(io::file::openReadOnly("/path/that/does/not/exist"), std::exception);
}

BOOST_AUTO_TEST_CASE(openWrite)
{
    const std::string SOME_DATA = "Hello, world!";
    std::unique_ptr<io::WriteBuffer> buffer = io::file::openWriteOnly(path);
    {
        google::protobuf::io::CodedOutputStream os(buffer.get());
        os.WriteString(SOME_DATA);
        BOOST_REQUIRE(!os.HadError());
    }
    buffer->close();
    BOOST_CHECK(!buffer->error());
    BOOST_CHECK_EQUAL(bunsan::testing::filesystem::read_data(path), SOME_DATA);
}

BOOST_AUTO_TEST_CASE(openWriteError)
{
    boost::filesystem::permissions(path, boost::filesystem::no_perms);
    BOOST_CHECK_THROW(io::file::openWriteOnly(path), std::exception);
}

BOOST_AUTO_TEST_SUITE_END() // file_

BOOST_AUTO_TEST_SUITE(filter_)

BOOST_FIXTURE_TEST_SUITE(gzip_, bunsan::testing::filesystem::tempfile)

const std::string SOME_DATA = "Hello, world!";

BOOST_AUTO_TEST_CASE(openRead)
{
    writeGzip(path, SOME_DATA);
    std::unique_ptr<io::ReadBuffer> buffer = io::filter::gzip::open(io::file::openReadOnly(path));
    {
        google::protobuf::io::CodedInputStream is(buffer.get());
        std::string data;
        BOOST_REQUIRE(is.ReadString(&data, SOME_DATA.size()));
        BOOST_CHECK_EQUAL(data, SOME_DATA);
    }
    buffer->close();
    BOOST_CHECK(buffer->closed());
    BOOST_CHECK(!buffer->error());
}

BOOST_AUTO_TEST_CASE(openReadError)
{
    bunsan::testing::filesystem::write_data(path, "NOT GZIP DATA");
    std::unique_ptr<io::ReadBuffer> buffer = io::file::openReadOnly(path);
    try {
        buffer = io::filter::gzip::open(std::move(buffer));
    } catch (std::exception &e) {
        BOOST_TEST_MESSAGE("Passed error test: " << path << ": " << e.what());
    }
    google::protobuf::io::CodedInputStream is(buffer.get());
    std::string data;
    BOOST_CHECK(!is.ReadString(&data, 10));
    BOOST_CHECK_THROW(buffer->checkError(), std::exception);
}

BOOST_AUTO_TEST_CASE(openWrite)
{
    std::unique_ptr<io::WriteBuffer> buffer = io::filter::gzip::open(io::file::openWriteOnly(path));
    {
        google::protobuf::io::CodedOutputStream os(buffer.get());
        os.WriteString(SOME_DATA);
        BOOST_REQUIRE(!os.HadError());
    }
    buffer->close();
    BOOST_CHECK(!buffer->error());
    checkIsGzip(path);
    BOOST_CHECK_EQUAL(readGzip(path), SOME_DATA);
}

BOOST_AUTO_TEST_CASE(gzipAppend)
{
    const std::string PREFIX_DATA = "Hello, ";
    const std::string POSTFIX_DATA = "world!";
    writeGzip(path, PREFIX_DATA);
    std::unique_ptr<io::WriteBuffer> buffer = io::filter::gzip::open(io::file::openAppendOnly(path));
    {
        google::protobuf::io::CodedOutputStream os(buffer.get());
        os.WriteString(POSTFIX_DATA);
        BOOST_REQUIRE(!os.HadError());
    }
    buffer->close();
    BOOST_CHECK(!buffer->error());
    checkIsGzip(path);
    BOOST_CHECK_EQUAL(readGzip(path), PREFIX_DATA + POSTFIX_DATA);
}

BOOST_AUTO_TEST_SUITE_END() // gzip_

BOOST_AUTO_TEST_SUITE_END() // filter_

BOOST_AUTO_TEST_SUITE_END() // io_
