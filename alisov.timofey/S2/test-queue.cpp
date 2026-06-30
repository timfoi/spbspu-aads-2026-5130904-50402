#include <boost/test/unit_test.hpp>
#include "queue.hpp"

BOOST_AUTO_TEST_CASE(new_queue_is_empty)
{
  alisov::Queue< int > q;
  BOOST_TEST(q.empty());
}

BOOST_AUTO_TEST_CASE(push_make_queue_non_empty)
{
  alisov::Queue< int > q;
  q.push(1);
  BOOST_TEST(!q.empty());
}
