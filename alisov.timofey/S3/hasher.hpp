#ifndef HASHER_HPP
#define HASHER_HPP
#include <cstddef>
#include <cstdint>

namespace alisov
{
  inline uint32_t leftRotate(uint32_t value, unsigned int shift)
  {
    return (value << shift) | (value >> (32 - shift));
  }
  size_t hashBytes(const uint8_t *message, size_t len);

  template < class T >
  struct SHA1
  {
    size_t operator()(const T &key) const
    {
      return hashBytes(reinterpret_cast< const uint8_t * >(&key), sizeof(T));
    }
  };
}
#endif
