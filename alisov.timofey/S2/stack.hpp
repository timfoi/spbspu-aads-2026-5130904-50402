#ifndef STACK_HPP
#define STACK_HPP

#include <cstddef>
#include <list.hpp>
#include <stdexcept>

namespace alisov
{
  template < class T >
  class Stack
  {
  public:
    void push(const T &rhs);
    T drop();
    T &get();
    const T &get() const;
    void pop();
    bool empty() const noexcept;
    size_t size() const noexcept;

  private:
    List< T > data_;
  };
}

template < class T >
void alisov::Stack< T >::push(const T &rhs)
{
  data_.push_front(rhs);
}

template < class T >
T alisov::Stack< T >::drop()
{
  if (empty()) {
    throw std::underflow_error("Stack is empty");
  }
  T value = data_.front();
  data_.pop_front();
  return value;
}

template < class T >
T &alisov::Stack< T >::get()
{
  if (empty()) {
    throw std::underflow_error("Stack is empty");
  }
  return data_.front();
}

template < class T >
const T &alisov::Stack< T >::get() const
{
  if (empty()) {
    throw std::underflow_error("Stack is empty");
  }
  return data_.front();
}

template < class T >
void alisov::Stack< T >::pop()
{
  if (empty()) {
    throw std::underflow_error("Stack is empty");
  }
  data_.pop_front();
}

template < class T >
bool alisov::Stack< T >::empty() const noexcept
{
  return data_.empty();
}

template < class T >
size_t alisov::Stack< T >::size() const noexcept
{
  return data_.size();
}

#endif
