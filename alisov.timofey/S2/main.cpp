#include <cctype>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include "queue.hpp"
#include "stack.hpp"

namespace alisov
{
  int getPriority(const std::string &op)
  {
    if (op == "<<") {
      return 3;
    }
    if (op == "*" || op == "/" || op == "%")
      return 2;
    if (op == "+" || op == "-")
      return 1;
    return 0;
  }

  bool isOperation(const std::string &s)
  {
    bool res = s == "<<";
    res = res || s == "*" || s == "/" || s == "%";
    res = res || s == "+" || s == "-";
    return res;
  }

  Queue< std::string > split(const std::string &expression)
  {
    Queue< std::string > res;
    std::string current;
    for (char ch : expression) {
      if (ch == ' ') {
        if (!current.empty()) {
          res.push(current);
          current.clear();
        }
      } else {
        current += ch;
      }
    }
    if (!current.empty()) {
      res.push(current);
    }
    return res;
  }

}
