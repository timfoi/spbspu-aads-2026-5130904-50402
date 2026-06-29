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

  Office() = default;
  Office(const Office &) = default;
  Office &operator=(const Office &) = default;
  Office(Office &&) noexcept = default;
  Office &operator=(Office &&) noexcept = default;
};

template < class K, class V >
struct Bucket
{
  K key;
  V value;
  int psl = -1;
  BucketState state = BucketState::Empty;

  Bucket() = default;
  Bucket(const Bucket &) = default;
  Bucket &operator=(const Bucket &) = default;
  Bucket(Bucket &&) noexcept = default;
  Bucket &operator=(Bucket &&) noexcept = default;
};

template < class K, class V >
class RobinHoodHashMap
{
private:
  std::vector< Bucket< K, V > > table;
  size_t table_size = 0;
  size_t table_capacity = 0;
  static constexpr double MAX_LOAD_FACTOR = 0.7;

  size_t get_hash(const K &key) const
  {
    if (table_capacity == 0)
      return 0;
    return std::hash< K >{}(key) % table_capacity;
  }

  void insert_no_resize(const K &key, V value)
  {
    Bucket< K, V > entry;
    entry.key = key;
    entry.value = std::move(value);
    entry.psl = 0;
    entry.state = BucketState::Occupied;

    size_t idx = get_hash(key);

    while (true) {
      if (table[idx].state == BucketState::Empty || table[idx].state == BucketState::Deleted) {
        table[idx] = std::move(entry);
        table_size++;
        return;
      }

      if (table[idx].state == BucketState::Occupied && table[idx].key == entry.key) {
        table[idx].value = std::move(entry.value);
        return;
      }

      if (entry.psl > table[idx].psl) {
        std::swap(entry, table[idx]);
      }

      idx = (idx + 1) % table_capacity;
      entry.psl++;
    }
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
        insert_no_resize(old_table[i].key, std::move(old_table[i].value));
      }
    }
  }

public:
  RobinHoodHashMap() = default;
  RobinHoodHashMap(const RobinHoodHashMap &) = default;
  RobinHoodHashMap &operator=(const RobinHoodHashMap &) = default;
  RobinHoodHashMap(RobinHoodHashMap &&) noexcept = default;
  RobinHoodHashMap &operator=(RobinHoodHashMap &&) noexcept = default;

  RobinHoodHashMap(size_t initial_capacity):
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

  void insert(const K &key, V value)
  {
    if (table_capacity == 0 || (double)table_size / table_capacity >= MAX_LOAD_FACTOR) {
      rehash();
    }
    insert_no_resize(key, std::move(value));
  }

  V *find(const K &key)
  {
    if (table_capacity == 0)
      return nullptr;
    size_t idx = get_hash(key);
    int current_psl = 0;

    while (true) {
      if (table[idx].state == BucketState::Empty)
        return nullptr;
      if (current_psl > table[idx].psl)
        return nullptr;

      if (table[idx].state == BucketState::Occupied && table[idx].key == key) {
        return &table[idx].value;
      }

      idx = (idx + 1) % table_capacity;
      current_psl++;
    }
  }

  bool remove(const K &key)
  {
    if (table_capacity == 0)
      return false;
    size_t idx = get_hash(key);
    int current_psl = 0;

    while (true) {
      if (table[idx].state == BucketState::Empty)
        return false;
      if (current_psl > table[idx].psl)
        return false;

      if (table[idx].state == BucketState::Occupied && table[idx].key == key) {
        table[idx].state = BucketState::Deleted;
        table[idx].psl = -1;
        table_size--;
        return true;
      }

      idx = (idx + 1) % table_capacity;
      current_psl++;
    }
  }
};

struct PostSystem
{
  std::string name;
  RobinHoodHashMap< std::string, Office > offices;

  PostSystem() = default;
  PostSystem(const PostSystem &) = default;
  PostSystem &operator=(const PostSystem &) = default;
  PostSystem(PostSystem &&) noexcept = default;
  PostSystem &operator=(PostSystem &&) noexcept = default;
};

class PostManager
{
private:
  RobinHoodHashMap< std::string, PostSystem > systems;
  RobinHoodHashMap< std::string, Mail > global_mails;

public:
  void make_post(const std::string &post_name)
  {
    if (systems.find(post_name) != nullptr) {
      std::cout << "<INVALID COMMAND>\n";
      return;
    }
    PostSystem new_system;
    new_system.name = post_name;
    systems.insert(post_name, std::move(new_system));
  }

  void show_post(const std::string &post_name)
  {
    PostSystem *sys = systems.find(post_name);
    if (sys == nullptr) {
      std::cout << "<INVALID COMMAND>\n";
      return;
    }

    size_t mail_count = 0;
    auto &mail_table = global_mails.get_raw_table();
    for (const auto &bucket : mail_table) {
      if (bucket.state == BucketState::Occupied && bucket.value.current_post == post_name) {
        mail_count++;
      }
    }

    std::cout << "<OFFICES: " << sys->offices.size() << ", MAILS: " << mail_count << ">\n";
  }
};

int main()
{}