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

  Queue< std::string > tokenize(const std::string &expression)
  {
    Queue< std::string > tokens;
    std::string current;
    for (char ch : expression) {
      if (ch == ' ') {
        if (!current.empty()) {
          tokens.push(current);
          current.clear();
        }
      } else {
        current += ch;
      }
    }
    if (!current.empty()) {
      tokens.push(current);
    }
    return tokens;
  }

  Queue< std::string > toPostfix(Queue< std::string > &infixTokens)
  {
    Queue< std::string > outputQueue;
    Stack< std::string > operatorStack;

    while (!infixTokens.empty()) {
      std::string token = infixTokens.drop();

      if (isNumber(token)) {
        outputQueue.push(token);
      } else if (token == "(") {
        operatorStack.push(token);
      } else if (token == ")") {
        while (!operatorStack.empty() && operatorStack.get() != "(") {
          outputQueue.push(operatorStack.drop());
        }
        if (operatorStack.empty()) {
          throw std::runtime_error("Mismatched parentheses");
        }
        operatorStack.pop();
      } else if (token.size() == 1
                 && (token[0] == '+' || token[0] == '-' || token[0] == '*' || token[0] == '/' || token[0] == '%')) {
        while (!operatorStack.empty() && getPriority(operatorStack.get()[0]) >= getPriority(token[0])) {
          outputQueue.push(operatorStack.drop());
        }
        operatorStack.push(token);
      } else {
        throw std::runtime_error("Invalid token");
      }
    }

    while (!operatorStack.empty()) {
      if (operatorStack.get() == "(" || operatorStack.get() == ")") {
        throw std::runtime_error("Mismatched parentheses");
      }
      outputQueue.push(operatorStack.drop());
    }

    return outputQueue;
  }
}
