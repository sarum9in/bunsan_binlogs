#define BOOST_TEST_MODULE io
#include <boost/test/unit_test.hpp>

#include "bunsan/binlogs/io/file/open.hpp"

#include "bunsan/testing/filesystem/read_data.hpp"
#include "bunsan/testing/filesystem/write_data.hpp"

#include <google/protobuf/io/coded_stream.h>

#include <boost/filesystem/operations.hpp>

using namespace bunsan::binlogs;

BOOST_AUTO_TEST_SUITE(io_)

struct FileFixture {
    ~FileFixture()
    {
        boost::filesystem::remove(tmp);
    }

    const boost::filesystem::path tmp =
        boost::filesystem::temp_directory_path() /
        boost::filesystem::unique_path();
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
        google::protobuf::io::CodedInputStream is(buffer->istream());
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
    std::unique_ptr<io::ReadBuffer> buffer = io::file::openReadOnly(tmp, &error);
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
        google::protobuf::io::CodedOutputStream os(buffer->ostream());
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

BOOST_AUTO_TEST_SUITE_END() // io_
