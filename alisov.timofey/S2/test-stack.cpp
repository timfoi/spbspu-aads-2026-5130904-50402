#define BOOST_TEST_MODULE Stack_Basic_Tests
#include <boost/test/included/unit_test.hpp>
#include "stack.hpp"

BOOST_AUTO_TEST_CASE(new_stack_empty)
{
  alisov::Stack< int > s;
  BOOST_TEST(s.empty());
}

BOOST_AUTO_TEST_CASE(push_make_stack_non_empty)
{
  alisov::Stack< int > s;
  s.push(1);
  BOOST_TEST(!s.empty());
}

BOOST_AUTO_TEST_CASE(get_method_return_last_elem)
{
  alisov::Stack< int > s;
  s.push(1);
  s.push(2);
  s.push(3);
  const alisov::Stack< int > rs = s;
  BOOST_TEST(rs.get() == 3);
}

BOOST_AUTO_TEST_CASE(pop_remove_elem)
{
  alisov::Stack< int > s;
  s.push(1);
  s.push(2);
  s.pop();
  BOOST_TEST(s.get() == 1);
}
