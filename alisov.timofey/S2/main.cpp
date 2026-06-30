#include <cctype>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include "queue.hpp"
#include "stack.hpp"

namespace alisov
{
  int getPriority(char op)
  {
    if (op == '+' || op == '-')
      return 1;
    if (op == '*' || op == '/' || op == '%')
      return 2;
    return 0;
  }

  bool isNumber(const std::string &s)
  {
    if (s.empty())
      return false;
    size_t start = 0;
    if (s[0] == '-' && s.size() > 1)
      start = 1;
    for (size_t i = start; i < s.size(); ++i) {
      if (!std::isdigit(s[i]))
        return false;
    }
    return true;
  }
}
