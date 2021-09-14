#pragma once
#include <string>
#include <vector>
#include <execution_nodes/connection.h>

namespace execution_nodes {
std::vector<std::string> split(const std::string &str, char delimiter);

inline std::string toString(const ConnectionDefinition &cn) {
  return cn.src.nodeName + ":" + cn.src.portName + " -> " + cn.dst.nodeName +
         ":" + cn.dst.portName;
}

size_t getIndexOfElement(const std::string &element,
                         const std::vector<std::string> &list, bool doThrow);
}