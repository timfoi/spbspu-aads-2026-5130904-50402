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

int main()
{}