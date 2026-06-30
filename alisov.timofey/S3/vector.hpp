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
    void reserve(size_t new_cap)
    {
      if (new_cap <= capacity_)
        return;
      T *new_data = reinterpret_cast< T * >(new char[new_cap * sizeof(T)]);
      for (size_t i = 0; i < size_; ++i) {
        new (&new_data[i]) T(std::move(data_[i]));
        data_[i].~T();
      }
      delete[] reinterpret_cast< char * >(data_);
      data_ = new_data;
      capacity_ = new_cap;
    }
  };
}
#endif
