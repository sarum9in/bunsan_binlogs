#define BOOST_TEST_MODULE equals
#include <boost/test/unit_test.hpp>

#include <bunsan/binlogs/detail/equals.hpp>
#include <bunsan/binlogs/tests/Message1.pb.h>
#include <bunsan/binlogs/tests/Message2.pb.h>

using namespace bunsan::binlogs;

BOOST_AUTO_TEST_SUITE(equals_)

BOOST_AUTO_TEST_CASE(_)
{
    tests::Message1 m1, m2;
    BOOST_CHECK(detail::equals(m1, m2));
    m1.set_key("key");
    BOOST_CHECK(!detail::equals(m1, m2));
    m2.set_key("key");
    BOOST_CHECK(detail::equals(m1, m2));
    m1.set_value("value");
    BOOST_CHECK(!detail::equals(m1, m2));
    m2.set_value("value");
    BOOST_CHECK(detail::equals(m1, m2));
    m1.Clear();
    BOOST_CHECK(!detail::equals(m1, m2));
    m2.Clear();
    BOOST_CHECK(detail::equals(m1, m2));
}

BOOST_AUTO_TEST_SUITE_END()
