#ifndef BSTREE_HPP
#define BSTREE_HPP
#include <algorithm>
#include <cstddef>
#include <new>
#include <stdexcept>
#include <utility>

namespace alisov
{
  template < class Key, class Value, class Compare >
  class BSTree;

  template < class Key, class Value, class Compare >
  class BSTConstIterator;

  template < class Key, class Value, class Compare >
  class BSTIterator
  {
  public:
    explicit BSTIterator(typename BSTree< Key, Value, Compare >::Node *curr):
      curr_(curr)
    {}

    BSTIterator(const BSTConstIterator< Key, Value, Compare > &rhs);

    bool operator==(const BSTIterator &rhs) const noexcept
    {
      return curr_ == rhs.curr_;
    }

    bool operator!=(const BSTIterator &rhs) const noexcept
    {
      return curr_ != rhs.curr_;
    }

    BSTIterator &operator++();

    std::pair< Key, Value & > operator*()
    {
      return {curr_->key, curr_->value};
    }

  private:
    typename BSTree< Key, Value, Compare >::Node *curr_;
    friend class BSTree< Key, Value, Compare >;
    friend class BSTConstIterator< Key, Value, Compare >;
  };

  template < class Key, class Value, class Compare >
  class BSTConstIterator
  {
  public:
    explicit BSTConstIterator(const typename BSTree< Key, Value, Compare >::Node *curr):
      curr_(curr)
    {}

    BSTConstIterator(const BSTIterator< Key, Value, Compare > &rhs);

    bool operator==(const BSTConstIterator &rhs) const noexcept
    {
      return curr_ == rhs.curr_;
    }

    bool operator!=(const BSTConstIterator &rhs) const noexcept
    {
      return curr_ != rhs.curr_;
    }

    BSTConstIterator &operator++();

    std::pair< Key, Value > operator*() const
    {
      return {curr_->key, curr_->value};
    }

  private:
    const typename BSTree< Key, Value, Compare >::Node *curr_;
    friend class BSTIterator< Key, Value, Compare >;
    friend class BSTree< Key, Value, Compare >;
  };
}
#endif
