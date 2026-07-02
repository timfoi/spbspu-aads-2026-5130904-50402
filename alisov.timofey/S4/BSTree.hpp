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
  template < class Key, class Value, class Compare = std::less< Key > >
  class BSTree
  {
  public:
    struct Node
    {
      Key key;
      Value value;
      Node *lt, *rt, *parent;
    };

    BSTree();
    ~BSTree();

    using const_iterator = BSTConstIterator< Key, Value, Compare >;
    using iterator = BSTIterator< Key, Value, Compare >;

    bool empty() const noexcept;

  private:
    friend class BSTIterator< Key, Value, Compare >;
    friend class BSTConstIterator< Key, Value, Compare >;

    Node fake_root_;
    Node *root_;
    Compare cmp_;

    static Node *nilNode();
    void clear(Node *curr);
  };

  template < class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::Node *BSTree< Key, Value, Compare >::nilNode()
  {
    static Node node{Key(), Value(), nullptr, nullptr, nullptr};
    node.lt = &node;
    node.rt = &node;
    node.parent = &node;
    return &node;
  }

  template < class Key, class Value, class Compare >
  BSTree< Key, Value, Compare >::BSTree():
    fake_root_{Key(), Value(), nullptr, nullptr, nullptr},
    root_(&fake_root_),
    cmp_(Compare())
  {
    root_->lt = nilNode();
    root_->rt = nilNode();
    root_->parent = nullptr;
  }

  template < class Key, class Value, class Compare >
  bool BSTree< Key, Value, Compare >::empty() const noexcept
  {
    return root_->lt == nilNode();
  }

  template < class Key, class Value, class Compare >
  void BSTree< Key, Value, Compare >::clear(Node *curr)
  {
    if (!curr || curr == root_ || curr == nilNode()) {
      return;
    }
    clear(curr->lt);
    clear(curr->rt);
    delete curr;
  }

  template < class Key, class Value, class Compare >
  BSTree< Key, Value, Compare >::~BSTree()
  {
    clear(root_->lt);
    root_->lt = nilNode();
  }
}
#endif
