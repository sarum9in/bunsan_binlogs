#define BOOST_TEST_MODULE log
#include <boost/test/unit_test.hpp>

#include "bunsan/binlogs/io/file/open.hpp"
#include "bunsan/binlogs/io/filter/gzip.hpp"
#include "bunsan/binlogs/LogFactory.hpp"
#include "bunsan/binlogs/tests/FileDescriptorSet.hpp"
#include "bunsan/binlogs/tests/Message1.pb.h"
#include "bunsan/binlogs/tests/Message2.pb.h"

#include "bunsan/testing/filesystem/tempfile.hpp"

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

BOOST_AUTO_TEST_CASE(_)
{
    std::string error;

    tests::Message1 msg1;
    tests::Message2 msg2;

    {
        std::unique_ptr<io::WriteBuffer> buffer = io::file::openWriteOnly(path, &error);
        BOOST_REQUIRE_MESSAGE(buffer, error);
        buffer = io::filter::gzip::open(std::move(buffer), &error);
        BOOST_REQUIRE_MESSAGE(buffer, error);
        std::unique_ptr<LogWriter> logWriter = openWriteOnly(std::move(buffer), getHeader(), &error);
        BOOST_REQUIRE_MESSAGE(logWriter, error);

        msg1.Clear();
        msg1.set_key("key1");
        msg1.set_value("value1");
        if (!logWriter->write(msg1, &error)) BOOST_FAIL(error);

        msg1.Clear();
        msg2.set_key("key2");
        msg2.add_values("value21");
        msg2.add_values("value22");
        msg2.add_values("value23");
        if (!logWriter->write(msg2, &error)) BOOST_FAIL(error);

        if (!logWriter->close(&error)) BOOST_FAIL(error);
    }

    {
        std::unique_ptr<io::ReadBuffer> buffer = io::file::openReadOnly(path, &error);
        BOOST_REQUIRE_MESSAGE(buffer, error);
        buffer = io::filter::gzip::open(std::move(buffer), &error);
        BOOST_REQUIRE_MESSAGE(buffer, error);
        std::unique_ptr<LogReader> logReader = openReadOnly(std::move(buffer), &error);
        BOOST_REQUIRE_MESSAGE(logReader, error);

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
        BOOST_REQUIRE_EQUAL(logReader->state(), LogReader::State::kEof);
    }
}

BOOST_AUTO_TEST_SUITE_END() // log_
