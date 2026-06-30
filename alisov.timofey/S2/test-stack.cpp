#define BOOST_TEST_MODULE Stack_Basic_Tests
#include <boost/test/included/unit_test.hpp>
#include "stack.hpp"

BOOST_AUTO_TEST_CASE(new_stack_empty)
{
  alisov::Stack< int > s;
  BOOST_TEST(s.empty());
}
