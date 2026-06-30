#ifndef COMMANDS_HPP
#define COMMANDS_HPP
#include <iostream>
#include "HashTable.hpp"
#include "graph.hpp"
#include "sort.hpp"

namespace alisov
{
  using GraphTable = HashTable< std::string, Graph, SHA1< std::string >, std::equal_to< std::string > >;

  inline void handleGraphs(const GraphTable &graphs)
  {
    Vector< std::string > keys = graphs.getAllKeys();
    sortVector(keys);
    for (size_t i = 0; i < keys.size(); ++i) {
      std::cout << keys[i] << "\n";
    }
  }

  inline void handleVertexes(const GraphTable &graphs, const std::string &gname)
  {
    if (!graphs.contains(gname)) {
      std::cout << "Invalid command\n";
      return;
    }
    Vector< std::string > v = graphs.at(gname).vertexes.getAllKeys();
    sortVector(v);
    for (size_t i = 0; i < v.size(); ++i) {
      std::cout << v[i] << "\n";
    }
  }
}
#endif
