#ifndef VECTOR_HPP
#define VECTOR_HPP
#include <cstddef>
#include <utility>

namespace alisov
{
  template < class T >
  class Vector
  {
  private:
    T *data_;
    size_t size_;
    size_t capacity_;

  public:
    Vector():
      data_(nullptr),
      size_(0),
      capacity_(0)
    {}
    ~Vector()
    {
      delete[] reinterpret_cast< char * >(data_);
    }
  };
}
#endif
