#define BOOST_TEST_MODULE log
#include <boost/test/unit_test.hpp>

#include "gzip.hpp"

#include <bunsan/binlogs/detail/directory_log.hpp>
#include <bunsan/binlogs/detail/files.hpp>
#include <bunsan/binlogs/io/file/open.hpp>
#include <bunsan/binlogs/io/filter/gzip.hpp>
#include <bunsan/binlogs/LogFactory.hpp>
#include <bunsan/binlogs/tests/FileDescriptorSet.hpp>
#include <bunsan/binlogs/tests/Message1.pb.h>
#include <bunsan/binlogs/tests/Message2.pb.h>

#include <bunsan/testing/filesystem/tempdir.hpp>
#include <bunsan/testing/filesystem/tempfile.hpp>
#include <bunsan/testing/filesystem/tempfiles.hpp>
#include <bunsan/testing/filesystem/write_data.hpp>

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
    BOOST_TEST_CHECKPOINT(BOOST_CURRENT_FUNCTION);
    return openReadOnly(path);
}

void writeTestData1(LogWriter *logWriter)
{
    tests::Message1 msg1;
    msg1.Clear();
    msg1.set_key("key1");
    msg1.set_value("value1");
    logWriter->write(msg1);
}

void writeTestData2(LogWriter *logWriter)
{
    tests::Message2 msg2;
    msg2.set_key("key2");
    msg2.add_values("value21");
    msg2.add_values("value22");
    msg2.add_values("value23");
    logWriter->write(msg2);
}

void writeTestData(LogWriter *logWriter)
{
    writeTestData1(logWriter);
    writeTestData2(logWriter);
}

void readTestData(LogReader *logReader)
{
    tests::Message1 msg1;
    tests::Message2 msg2;

    // TODO implement operator<<() and use BOOST_CHECK_EQUAL()
    BOOST_CHECK(logReader->messageTypePool().header() == getHeader());
    const MessageType *next = logReader->nextMessageType();
    BOOST_REQUIRE(next);
    BOOST_REQUIRE_EQUAL(next->typeName(), "bunsan.binlogs.tests.Message1");
    logReader->read(msg1);
    BOOST_REQUIRE_EQUAL(msg1.key(), "key1");
    BOOST_REQUIRE_EQUAL(msg1.value(), "value1");

    next = logReader->nextMessageType();
    BOOST_REQUIRE(next);
    BOOST_REQUIRE_EQUAL(next->typeName(), "bunsan.binlogs.tests.Message2");
    logReader->read(msg2);
    BOOST_REQUIRE_EQUAL(msg2.key(), "key2");
    BOOST_REQUIRE_EQUAL(msg2.values_size(), 3);
    BOOST_REQUIRE_EQUAL(msg2.values(0), "value21");
    BOOST_REQUIRE_EQUAL(msg2.values(1), "value22");
    BOOST_REQUIRE_EQUAL(msg2.values(2), "value23");

    next = logReader->nextMessageType();
    BOOST_CHECK(!next);
    BOOST_CHECK(logReader->eof());
}

void readTestData(const boost::filesystem::path &path)
{
    auto logReader = openLogReader(path);
    readTestData(logReader.get());
    BOOST_REQUIRE_EQUAL(logReader->state(), LogReader::State::kEof);
}

BOOST_AUTO_TEST_CASE(anonymous)
{
    std::unique_ptr<LogWriter> logWriter = openWriteOnly(detail::openFileWriteOnly(path), getHeader());
    writeTestData(logWriter.get());
    logWriter->close();
    readTestData(path);
}

BOOST_AUTO_TEST_CASE(doubleClose)
{
    {
        const auto logWriter = openWriteOnly(path, getHeader());
        writeTestData(logWriter.get());
        logWriter->close();
        logWriter->close();
    }
    {
        const auto logReader = openReadOnly(path);
        readTestData(logReader.get());
        logReader->close();
        logReader->close();
    }
}

BOOST_AUTO_TEST_CASE(readFail)
{
    // TODO test for State::kFail log reading
}

BOOST_FIXTURE_TEST_CASE(named, bunsan::testing::filesystem::tempfiles)
{
    const boost::filesystem::path tmp1 = allocate(), tmp2 = allocate(), tmp3 = allocate();

    std::unique_ptr<NamedLogWriter> namedLogWriter = openWriteOnly(tmp1, getHeader());

    writeTestData(namedLogWriter.get());
    namedLogWriter->reopen(tmp2);
    readTestData(tmp1);
    writeTestData(namedLogWriter.get());
    namedLogWriter->close();
    readTestData(tmp2);

    namedLogWriter->open(tmp1);
    writeTestData(namedLogWriter.get());
    namedLogWriter->rotate(tmp2);
    readTestData(tmp2);
    writeTestData(namedLogWriter.get());

    namedLogWriter->reopen(tmp3);
    readTestData(tmp1);
    writeTestData(namedLogWriter.get());
    namedLogWriter->close();
    readTestData(tmp3);
}

struct AppendFixture: bunsan::testing::filesystem::tempfile {
    std::unique_ptr<NamedLogWriter> namedLogWriter = newWriter(getHeader());
};

BOOST_FIXTURE_TEST_SUITE(append, AppendFixture)

BOOST_AUTO_TEST_CASE(internal)
{
    namedLogWriter->open(path);
    writeTestData1(namedLogWriter.get());
    namedLogWriter->close();
    namedLogWriter->append(path);
    writeTestData2(namedLogWriter.get());
    namedLogWriter->close();
    readTestData(path);
}

BOOST_AUTO_TEST_CASE(external)
{
    namedLogWriter->open(path);
    writeTestData1(namedLogWriter.get());
    namedLogWriter->close();
    namedLogWriter = openAppendOnly(path, getHeader());
    writeTestData2(namedLogWriter.get());
    namedLogWriter->close();
    readTestData(path);
}

BOOST_AUTO_TEST_CASE(incompatibleHeader)
{
    namedLogWriter->open(path);
    writeTestData1(namedLogWriter.get());
    namedLogWriter->close();
    Header header = getHeader();
    BOOST_REQUIRE_GE(header.types.size(), 2);
    swap(header.types[0], header.types[1]); // shuffle
    BOOST_REQUIRE(header != getHeader()); // TODO implement Header's operator<<() and use NE
    BOOST_CHECK_THROW(openAppendOnly(path, header), std::exception);
}

BOOST_AUTO_TEST_SUITE(corrupted)

BOOST_AUTO_TEST_CASE(gzip)
{
    namedLogWriter = openWriteOnly(path, getHeader());
    writeTestData1(namedLogWriter.get());
    namedLogWriter->close();
    const auto size = boost::filesystem::file_size(path);
    BOOST_REQUIRE(size > 2);
    boost::filesystem::resize_file(path, size / 2);
    BOOST_CHECK_THROW(namedLogWriter->append(path), std::exception);
}

BOOST_AUTO_TEST_CASE(footer)
{
    namedLogWriter = openWriteOnly(path, getHeader());
    writeTestData1(namedLogWriter.get());
    namedLogWriter->close();
    std::string data = readGzip(path);
    BOOST_REQUIRE(data.size() > 2);
    data.resize(data.size() - 2);
    writeGzip(path, data);
    BOOST_CHECK_THROW(namedLogWriter->append(path), std::exception);
}

BOOST_AUTO_TEST_SUITE_END() // corrupted

BOOST_AUTO_TEST_SUITE_END() // append

BOOST_FIXTURE_TEST_SUITE(directory_log_, bunsan::testing::filesystem::tempdir)

BOOST_AUTO_TEST_CASE(listDir)
{
    const std::vector<std::string> paths = {
        "bunsan_binlog_000000000.gz",
        "bunsan_binlog_000000001.gz",
        "bunsan_binlog_000000002.gz",
        "bunsan_binlog_000000003.gz",
        "bunsan_binlog_000000005.gz",
        "bunsan_binlog_000010003.gz",
        "bunsan_binlog_999909999.gz",
        "bunsan_binlog_999999999.gz",
    };
    const std::vector<std::string> nonLogPaths = {
        "123",
        "bunsan_binlog_0.gz",
        "bunsan_binlog_0000000000.gz",
        "bunsan_binlog_0000000001.gz",
        "bunsan_binlog_1000000000.gz",
    };
    for (const std::string &name: paths) {
        bunsan::testing::filesystem::write_data(path / name, "");
    }
    for (const std::string &name: nonLogPaths) {
        bunsan::testing::filesystem::write_data(path / name, "");
    }
    const std::vector<boost::filesystem::path> result = detail::directory_log::listDir(path);
    BOOST_REQUIRE_EQUAL(result.size(), paths.size());
    for (std::size_t i = 0; i < paths.size(); ++i) {
        BOOST_CHECK_EQUAL(result[i], path / paths[i]);
    }
}

BOOST_AUTO_TEST_CASE(nextPath)
{
    const std::vector<std::string> paths = {
        "bunsan_binlog_000000000.gz",
        "bunsan_binlog_000000001.gz",
        "bunsan_binlog_000000002.gz",
        "bunsan_binlog_000000003.gz",
    };
    for (const boost::filesystem::path &next: paths) {
        BOOST_CHECK_EQUAL(path / next, detail::directory_log::nextPath(path));
        bunsan::testing::filesystem::write_data(path / next, "");
    }
    bunsan::testing::filesystem::write_data(path / "bunsan_binlog_999999999.gz", "");
    BOOST_CHECK_THROW(detail::directory_log::nextPath(path), detail::directory_log::TooManyLogFilesError);
}

BOOST_AUTO_TEST_SUITE_END() // directory_log_

BOOST_AUTO_TEST_SUITE_END() // log_
