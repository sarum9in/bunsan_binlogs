#define BOOST_TEST_MODULE log
#include <boost/test/unit_test.hpp>

#include "gzip.hpp"

#include <bunsan/binlogs/io/file/open.hpp>
#include <bunsan/binlogs/io/filter/gzip.hpp>
#include <bunsan/binlogs/LogFactory.hpp>
#include <bunsan/binlogs/tests/FileDescriptorSet.hpp>
#include <bunsan/binlogs/tests/Message1.pb.h>
#include <bunsan/binlogs/tests/Message2.pb.h>

#include <bunsan/testing/filesystem/tempfile.hpp>
#include <bunsan/testing/filesystem/tempfiles.hpp>

#include <boost/filesystem/operations.hpp>

BOOST_FIXTURE_TEST_SUITE(log_, bunsan::testing::filesystem::tempfile)

using namespace bunsan::binlogs;

Header getHeader()
{
    Header header = {
        .proto = tests::fileDescriptorSet(),
        .types = {
            "bunsan.binlogs.tests.Message1",
            "bunsan.binlogs.tests.Message2",
        },
    };
    return header;
}

std::unique_ptr<LogReader> openLogReader(const boost::filesystem::path &path)
{
    std::string error;
    std::unique_ptr<LogReader> logReader = openReadOnly(path, &error);
    BOOST_REQUIRE_MESSAGE(logReader, error);
    return logReader;
}

void writeTestData1(LogWriter *logWriter)
{
    std::string error;
    tests::Message1 msg1;
    msg1.Clear();
    msg1.set_key("key1");
    msg1.set_value("value1");
    if (!logWriter->write(msg1, &error)) BOOST_FAIL(error);
}

void writeTestData2(LogWriter *logWriter)
{
    std::string error;
    tests::Message2 msg2;
    msg2.set_key("key2");
    msg2.add_values("value21");
    msg2.add_values("value22");
    msg2.add_values("value23");
    if (!logWriter->write(msg2, &error)) BOOST_FAIL(error);
}

void writeTestData(LogWriter *logWriter)
{
    writeTestData1(logWriter);
    writeTestData2(logWriter);
}

void readTestData(LogReader *logReader)
{
    std::string error;
    tests::Message1 msg1;
    tests::Message2 msg2;

    // TODO implement operator<<() and use BOOST_CHECK_EQUAL()
    BOOST_CHECK(logReader->messageTypePool().header() == getHeader());
    const MessageType *next = logReader->nextMessageType(&error);
    BOOST_REQUIRE_MESSAGE(next, error);
    BOOST_REQUIRE_EQUAL(next->typeName(), "bunsan.binlogs.tests.Message1");
    if (!logReader->read(msg1, &error)) BOOST_FAIL(error);
    BOOST_REQUIRE_EQUAL(msg1.key(), "key1");
    BOOST_REQUIRE_EQUAL(msg1.value(), "value1");

    next = logReader->nextMessageType(&error);
    BOOST_REQUIRE_MESSAGE(next, error);
    BOOST_REQUIRE_EQUAL(next->typeName(), "bunsan.binlogs.tests.Message2");
    if (!logReader->read(msg2, &error)) BOOST_FAIL(error);
    BOOST_REQUIRE_EQUAL(msg2.key(), "key2");
    BOOST_REQUIRE_EQUAL(msg2.values_size(), 3);
    BOOST_REQUIRE_EQUAL(msg2.values(0), "value21");
    BOOST_REQUIRE_EQUAL(msg2.values(1), "value22");
    BOOST_REQUIRE_EQUAL(msg2.values(2), "value23");

    next = logReader->nextMessageType(&error);
    BOOST_REQUIRE(!next);
}

void readTestData(const boost::filesystem::path &path)
{
    auto logReader = openLogReader(path);
    readTestData(logReader.get());
    BOOST_REQUIRE_EQUAL(logReader->state(), LogReader::State::kEof);
}

BOOST_AUTO_TEST_CASE(anonymous)
{
    std::string error;

    std::unique_ptr<io::WriteBuffer> buffer = io::file::openWriteOnly(path, &error);
    BOOST_REQUIRE_MESSAGE(buffer, error);
    buffer = io::filter::gzip::open(std::move(buffer), &error);
    BOOST_REQUIRE_MESSAGE(buffer, error);
    std::unique_ptr<LogWriter> logWriter = openWriteOnly(std::move(buffer), getHeader(), &error);
    BOOST_REQUIRE_MESSAGE(logWriter, error);
    writeTestData(logWriter.get());
    if (!logWriter->close(&error)) BOOST_FAIL(error);

    readTestData(path);
}

BOOST_AUTO_TEST_CASE(readFail)
{
    // TODO test for State::kFail log reading
}

BOOST_FIXTURE_TEST_CASE(named, bunsan::testing::filesystem::tempfiles)
{
    const boost::filesystem::path tmp1 = allocate(), tmp2 = allocate(), tmp3 = allocate();
    std::string error;

    std::unique_ptr<NamedLogWriter> namedLogWriter = openWriteOnly(tmp1, getHeader(), &error);
    BOOST_REQUIRE_MESSAGE(namedLogWriter, error);

    writeTestData(namedLogWriter.get());
    if (!namedLogWriter->reopen(tmp2, &error)) BOOST_FAIL(error);
    readTestData(tmp1);
    writeTestData(namedLogWriter.get());
    if (!namedLogWriter->close(&error)) BOOST_FAIL(error);
    readTestData(tmp2);

    if (!namedLogWriter->open(tmp1, &error)) BOOST_FAIL(error);
    writeTestData(namedLogWriter.get());
    if (!namedLogWriter->rotate(tmp2, &error)) BOOST_FAIL(error);
    readTestData(tmp2);
    writeTestData(namedLogWriter.get());

    if (!namedLogWriter->reopen(tmp3, &error)) BOOST_FAIL(error);
    readTestData(tmp1);
    writeTestData(namedLogWriter.get());
    if (!namedLogWriter->close(&error)) BOOST_FAIL(error);
    readTestData(tmp3);
}

struct AppendFixture: bunsan::testing::filesystem::tempfile {
    std::string error;
    std::unique_ptr<NamedLogWriter> namedLogWriter = newWriter(getHeader());
};

BOOST_FIXTURE_TEST_SUITE(append, AppendFixture)

BOOST_AUTO_TEST_CASE(internal)
{
    if (!namedLogWriter->open(path, &error)) BOOST_FAIL(error);
    writeTestData1(namedLogWriter.get());
    if (!namedLogWriter->close(&error)) BOOST_FAIL(error);
    if (!namedLogWriter->append(path, &error)) BOOST_FAIL(error);
    writeTestData2(namedLogWriter.get());
    if (!namedLogWriter->close(&error)) BOOST_FAIL(error);
    readTestData(path);
}

BOOST_AUTO_TEST_CASE(external)
{
    if (!namedLogWriter->open(path, &error)) BOOST_FAIL(error);
    writeTestData1(namedLogWriter.get());
    if (!namedLogWriter->close(&error)) BOOST_FAIL(error);
    namedLogWriter = openAppendOnly(path, getHeader(), &error);
    BOOST_REQUIRE_MESSAGE(namedLogWriter, error);
    writeTestData2(namedLogWriter.get());
    if (!namedLogWriter->close(&error)) BOOST_FAIL(error);
    readTestData(path);
}

BOOST_AUTO_TEST_CASE(incompatibleHeader)
{
    if (!namedLogWriter->open(path, &error)) BOOST_FAIL(error);
    writeTestData1(namedLogWriter.get());
    if (!namedLogWriter->close(&error)) BOOST_FAIL(error);
    Header header = getHeader();
    BOOST_REQUIRE_GE(header.types.size(), 2);
    swap(header.types[0], header.types[1]); // shuffle
    BOOST_REQUIRE(header != getHeader()); // TODO implement Header's operator<<() and use NE
    namedLogWriter = openAppendOnly(path, header, &error);
    BOOST_CHECK_MESSAGE(!namedLogWriter, error);
}

BOOST_AUTO_TEST_SUITE(corrupted)

BOOST_AUTO_TEST_CASE(gzip)
{
    namedLogWriter = openWriteOnly(path, getHeader(), &error);
    BOOST_REQUIRE_MESSAGE(namedLogWriter, error);
    writeTestData1(namedLogWriter.get());
    if (!namedLogWriter->close(&error)) BOOST_FAIL(error);
    const auto size = boost::filesystem::file_size(path);
    BOOST_REQUIRE(size > 2);
    boost::filesystem::resize_file(path, size / 2);
    const bool ret = namedLogWriter->append(path, &error);
    BOOST_CHECK_MESSAGE(!ret, error);
}

BOOST_AUTO_TEST_CASE(footer)
{
    namedLogWriter = openWriteOnly(path, getHeader(), &error);
    BOOST_REQUIRE_MESSAGE(namedLogWriter, error);
    writeTestData1(namedLogWriter.get());
    if (!namedLogWriter->close(&error)) BOOST_FAIL(error);
    std::string data = readGzip(path);
    BOOST_REQUIRE(data.size() > 2);
    data.resize(data.size() - 2);
    writeGzip(path, data);
    const bool ret = namedLogWriter->append(path, &error);
    BOOST_CHECK_MESSAGE(!ret, error);
}

BOOST_AUTO_TEST_SUITE_END() // corrupted

BOOST_AUTO_TEST_SUITE_END() // append

BOOST_AUTO_TEST_SUITE_END() // log_
