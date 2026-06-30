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
  void add_office(const std::string &post_name, const std::string &office_name)
  {
    PostSystem *sys = systems.find(post_name);
    if (sys == nullptr || sys->offices.find(office_name) != nullptr) {
      std::cout << "<INVALID COMMAND>\n";
      return;
    }
    Office off;
    off.name = office_name;
    sys->offices.insert(office_name, std::move(off));
  }

  void show_office(const std::string &post_name, const std::string &office_name)
  {
    PostSystem *sys = systems.find(post_name);
    if (sys == nullptr) {
      std::cout << "<INVALID COMMAND>\n";
      return;
    }
    Office *off = sys->offices.find(office_name);
    if (off == nullptr) {
      std::cout << "<INVALID COMMAND>\n";
      return;
    }
    std::cout << "<NAME: " << off->name << ", MAILS: " << off->local_mail_ids.size() << ">\n";
  }

  void add_mail(const std::string &post_name, const std::string &track_id, const std::string &office_name,
                double weight)
  {
    PostSystem *sys = systems.find(post_name);
    if (sys == nullptr || weight <= 0) {
      std::cout << "<INVALID COMMAND>\n";
      return;
    }
    Office *off = sys->offices.find(office_name);
    if (off == nullptr || global_mails.find(track_id) != nullptr) {
      std::cout << "<INVALID COMMAND>\n";
      return;
    }

    Mail m;
    m.track_id = track_id;
    m.current_office = office_name;
    m.current_post = post_name;
    m.weight = weight;

    global_mails.insert(track_id, std::move(m));
    off->local_mail_ids.push_back(track_id);
  }

  void show_mail(const std::string &post_name, const std::string &track_id)
  {
    PostSystem *sys = systems.find(post_name);
    Mail *m = global_mails.find(track_id);
    if (sys == nullptr || m == nullptr || m->current_post != post_name) {
      std::cout << "<INVALID COMMAND>\n";
      return;
    }
    std::cout << "<TRACK: " << m->track_id << ", OFFICE: " << m->current_office << ", WEIGHT: " << m->weight << ">\n";
  }
  void move_mail(const std::string &post_name, const std::string &track_id, const std::string &new_office_name)
  {
    PostSystem *sys = systems.find(post_name);
    Mail *m = global_mails.find(track_id);
    if (sys == nullptr || m == nullptr || m->current_post != post_name) {
      std::cout << "<INVALID COMMAND>\n";
      return;
    }
    Office *old_off = sys->offices.find(m->current_office);
    Office *new_off = sys->offices.find(new_office_name);
    if (old_off == nullptr || new_off == nullptr) {
      std::cout << "<INVALID COMMAND>\n";
      return;
    }

    auto &v = old_off->local_mail_ids;
    v.erase(std::remove(v.begin(), v.end(), track_id), v.end());

    new_off->local_mail_ids.push_back(track_id);
    m->current_office = new_office_name;
  }

  void find_weight(const std::string &post_name, double left, double right)
  {
    if (systems.find(post_name) == nullptr || left > right) {
      std::cout << "<INVALID COMMAND>\n";
      return;
    }

    std::vector< Mail > result;
    auto &mail_table = global_mails.get_raw_table();
    for (const auto &bucket : mail_table) {
      if (bucket.state == BucketState::Occupied && bucket.value.current_post == post_name) {
        if (bucket.value.weight >= left && bucket.value.weight <= right) {
          result.push_back(bucket.value);
        }
      }
    }

    std::sort(result.begin(), result.end(), [](const Mail &a, const Mail &b) {
      return a.track_id < b.track_id;
    });

    std::cout << "<MAILS:";
    for (size_t i = 0; i < result.size(); ++i) {
      std::cout << " " << result[i].track_id << " (" << result[i].weight << ")";
      if (i + 1 < result.size())
        std::cout << ",";
    }
    std::cout << ">\n";
  }

  void link_offices(const std::string &post_name, const std::string &off1, const std::string &off2, double dist)
  {
    PostSystem *sys = systems.find(post_name);
    if (sys == nullptr || dist < 0) {
      std::cout << "<INVALID COMMAND>\n";
      return;
    }
    Office *o1 = sys->offices.find(off1);
    Office *o2 = sys->offices.find(off2);
    if (o1 == nullptr || o2 == nullptr) {
      std::cout << "<INVALID COMMAND>\n";
      return;
    }

    o1->links.push_back({off2, dist, dist * 1.5, dist * 0.1});
    o2->links.push_back({off1, dist, dist * 1.5, dist * 0.1});

    std::cout << "<LINKED: " << off1 << " - " << off2 << ", DISTANCE: " << dist << ">\n";
  }

  struct RouteResult
  {
    std::vector< std::string > path;
    double total_metric = -1;
  };

  RouteResult calculate(PostSystem *sys, const std::string &start, const std::string &target, int mode)
  {
    struct NodeState
    {
      std::string name;
      double dist = 1e9;
      std::string parent = "";
      bool visited = false;
    };

    std::vector< NodeState > states;
    auto &offices_table = sys->offices.get_raw_table();
    for (const auto &b : offices_table) {
      if (b.state == BucketState::Occupied) {
        states.push_back({b.key, 1e9, "", false});
      }
    }

    auto get_state = [&states](const std::string &name) -> NodeState * {
      for (auto &s : states)
        if (s.name == name)
          return &s;
      return nullptr;
    };

    NodeState *start_state = get_state(start);
    if (!start_state)
      return {{}, -1};

    start_state->dist = 0;

    for (size_t i = 0; i < states.size(); ++i) {
      NodeState *min_node = nullptr;
      for (auto &s : states) {
        if (!s.visited && (min_node == nullptr || s.dist < min_node->dist)) {
          min_node = &s;
        }
      }

      if (min_node == nullptr || min_node->dist >= 1e9)
        break;
      min_node->visited = true;

      if (min_node->name == target)
        break;

      Office *off = sys->offices.find(min_node->name);
      if (!off)
        continue;

      for (const auto &link : off->links) {
        NodeState *to_state = get_state(link.to_office);
        if (!to_state || to_state->visited)
          continue;

        double weight = link.distance;
        if (mode == 1)
          weight = link.cost;
        if (mode == 2)
          weight = link.time;

        if (min_node->dist + weight < to_state->dist) {
          to_state->dist = min_node->dist + weight;
          to_state->parent = min_node->name;
        }
      }
    }

    NodeState *target_state = get_state(target);
    if (!target_state || target_state->dist >= 1e9)
      return {{}, -1};

    std::vector< std::string > path;
    std::string curr = target;
    while (curr != "") {
      path.push_back(curr);
      curr = get_state(curr)->parent;
    }
    std::reverse(path.begin(), path.end());

    return {path, target_state->dist};
  }
  void route_mail(const std::string &post_name, const std::string &track_id, const std::string &target_office,
                  const std::string &type)
  {
    PostSystem *sys = systems.find(post_name);
    Mail *m = global_mails.find(track_id);
    if (sys == nullptr || m == nullptr || m->current_post != post_name) {
      std::cout << "<INVALID COMMAND>" << std::endl;
      return;
    }
    if (sys->offices.find(target_office) == nullptr) {
      std::cout << "<INVALID COMMAND>" << std::endl;
      return;
    }

    int mode = 0;
    std::string metric_name = "DISTANCE";
    if (type == "cheap") {
      mode = 1;
      metric_name = "COST";
    } else if (type == "fast") {
      mode = 2;
      metric_name = "TIME";
    } else if (type != "short") {
      std::cout << "<INVALID COMMAND>" << std::endl;
      return;
    }

    RouteResult res = calculate(sys, m->current_office, target_office, mode);
    if (res.path.empty()) {
      std::cout << "<INVALID COMMAND>" << std::endl;
      return;
    }

    std::cout << "<ROUTED: " << track_id << ", PATH: ";
    for (size_t i = 0; i < res.path.size(); ++i) {
      std::cout << res.path[i];
      if (i + 1 < res.path.size())
        std::cout << " -> ";
    }
    std::cout << ", TOTAL " << metric_name << ": " << res.total_metric << ">" << std::endl;

    move_mail(post_name, track_id, target_office);
  }
};

void process_commands()
{
  PostManager manager;
  std::string cmd;

  while (std::cin >> cmd) {
    if (cmd == "make-post") {
      std::string name;
      std::cin >> name;
      manager.make_post(name);
    } else if (cmd == "show-post") {
      std::string name;
      std::cin >> name;
      manager.show_post(name);
    } else if (cmd == "add-office") {
      std::string p_name, o_name;
      std::cin >> p_name >> o_name;
      manager.add_office(p_name, o_name);
    } else if (cmd == "show-office") {
      std::string p_name, o_name;
      std::cin >> p_name >> o_name;
      manager.show_office(p_name, o_name);
    } else if (cmd == "add-mail") {
      std::string p_name, t_id, o_name;
      double w;
      std::cin >> p_name >> t_id >> o_name >> w;
      manager.add_mail(p_name, t_id, o_name, w);
    } else if (cmd == "show-mail") {
      std::string p_name, t_id;
      std::cin >> p_name >> t_id;
      manager.show_mail(p_name, t_id);
    } else if (cmd == "move-mail") {
      std::string p_name, t_id, new_o;
      std::cin >> p_name >> t_id >> new_o;
      manager.move_mail(p_name, t_id, new_o);
    } else if (cmd == "find-weight") {
      std::string p_name;
      double l, r;
      std::cin >> p_name >> l >> r;
      manager.find_weight(p_name, l, r);
    } else if (cmd == "link-offices") {
      std::string p_name, o1, o2;
      double d;
      std::cin >> p_name >> o1 >> o2 >> d;
      manager.link_offices(p_name, o1, o2, d);
    } else if (cmd == "route-mail") {
      std::string p_name, t_id, t_off, type;
      std::cin >> p_name >> t_id >> t_off >> type;
      manager.route_mail(p_name, t_id, t_off, type);
    } else {
      std::cout << "<INVALID COMMAND>\n";
    }
  }
}

int main()
{
  process_commands();
}
