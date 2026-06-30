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

  Queue< std::string > toPostfix(Queue< std::string > &infixTokens)
  {
    Queue< std::string > outputQueue;
    Stack< std::string > operatorStack;

    while (!infixTokens.empty()) {
      std::string val = infixTokens.get();
      if (val == "(") {
        operatorStack.push(val);
      } else if (val == ")") {
        std::string op = operatorStack.get();
        while (isOperation(op)) {
          outputQueue.push(op);
          operatorStack.pop();
          op = operatorStack.get();
        }
        if (operatorStack.get() == "(") {
          operatorStack.pop();
        }
      } else if (isOperation(val)) {
        if (!operatorStack.empty()) {
          std::string op = operatorStack.get();
          while (op != "(" && getPriority(op) >= getPriority(val)) {
            outputQueue.push(op);
            operatorStack.pop();
            if (operatorStack.empty()) {
              break;
            }
            op = operatorStack.get();
          }
        }
        operatorStack.push(val);
      } else {
        outputQueue.push(val);
      }
      infixTokens.pop();
    }

    while (!operatorStack.empty()) {
      outputQueue.push(operatorStack.get());
      operatorStack.pop();
    }

    return outputQueue;
  }

}
