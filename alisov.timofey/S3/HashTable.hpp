#ifndef HASHTABLE_HPP
#define HASHTABLE_HPP

#include <cstddef>
#include <stdexcept>
#include <utility>
#include "vector.hpp"

namespace alisov
{
  template < class Key, class Value, class Hash, class Equal >
  class HashTable
  {
  private:
    struct Node
    {
      Key key;
      Value value;
      Node():
        key(Key()),
        value(Value())
      {}
      Node(Key k, Value v):
        key(k),
        value(v)
      {}
    };

    Vector< Vector< Node > > buckets_;
    size_t element_count_;
    size_t slots_count_;
    Hash hasher_;
    Equal equal_;

    size_t getBucketIndex(const Key &k) const
    {
      return slots_count_ == 0 ? 0 : hasher_(k) % slots_count_;
    }

  public:
    HashTable():
      element_count_(0),
      slots_count_(0)
    {}

    explicit HashTable(size_t slots):
      element_count_(0),
      slots_count_(slots)
    {
      buckets_.reserve(slots);
      for (size_t i = 0; i < slots; ++i) {
        buckets_.push_back(Vector< Node >());
      }
    }

    ~HashTable() = default;

    size_t size() const
    {
      return element_count_;
    }

    size_t slotsCount() const
    {
      return slots_count_;
    }

    bool empty() const
    {
      return element_count_ == 0;
    }
  };
}

#endif
