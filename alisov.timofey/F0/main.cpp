#include <algorithm>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

enum class BucketState { Empty, Occupied, Deleted };

struct Mail
{
  std::string track_id;
  std::string current_office;
  std::string current_post;
  double weight = 0.0;
};

struct Link
{
  std::string to_office;
  double distance = 0.0;
  double cost = 0.0;
  double time = 0.0;
};

struct Office
{
  std::string name;
  std::vector< Link > links;
  std::vector< std::string > local_mail_ids;
};

template < class K, class V >
struct Bucket
{
  K key;
  V value;
  int psl = -1; // Probe Sequence Length
  BucketState state = BucketState::Empty;
};

template < class K, class V >
class RobinHoodHashMap
{
private:
  std::vector< Bucket< K, V > > table;
  size_t table_size = 0;
  size_t table_capacity = 0;
  const double MAX_LOAD_FACTOR = 0.7;

  size_t get_hash(const K &key) const
  {
    if (table_capacity == 0)
      return 0;
    return std::hash< K >{}(key) % table_capacity;
  }

  void rehash()
  {
    size_t old_capacity = table_capacity;
    auto old_table = std::move(table);

    table_capacity = (old_capacity == 0) ? 8 : old_capacity * 2;
    table.assign(table_capacity, Bucket< K, V >());
    table_size = 0;

    for (size_t i = 0; i < old_capacity; ++i) {
      if (old_table[i].state == BucketState::Occupied) {
        insert_no_resize(old_table[i].key, old_table[i].value);
      }
    }
  }

  void insert_no_resize(const K &key, const V &value);

public:
  RobinHoodHashMap(size_t initial_capacity = 8):
    table_capacity(initial_capacity)
  {
    table.resize(table_capacity);
  }

  size_t size() const
  {
    return table_size;
  }
  bool empty() const
  {
    return table_size == 0;
  }
  const std::vector< Bucket< K, V > > &get_raw_table() const
  {
    return table;
  }
};

int main()
{}