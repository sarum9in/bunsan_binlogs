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
    google::protobuf::io::CodedInputStream cis(buffer->istream());
    std::string data;
    BOOST_REQUIRE(cis.ReadString(&data, SOME_DATA.size()));
    BOOST_CHECK_EQUAL(data, SOME_DATA);
    buffer->close();
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

BOOST_AUTO_TEST_SUITE_END() // file_

BOOST_AUTO_TEST_SUITE_END() // io_
