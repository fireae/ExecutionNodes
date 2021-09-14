#pragma once
#include <chrono>
#include <execution_nodes/connection.h>
#include <execution_nodes/internal/topological_sort.h>
#include <future>
#include <set>
#include <string>
#include <vector>

namespace execution_nodes {

namespace helpers {

std::vector<std::string> split(const std::string &str, char delimiter);

inline std::string toString(const ConnectionDefinition &cn) {
  return cn.src.nodeName + ":" + cn.src.portName + " -> " + cn.dst.nodeName +
         ":" + cn.dst.portName;
}

size_t getIndexOfElement(const std::string &element,
                         const std::vector<std::string> &list, bool doThrow);

inline bool isSubset(const std::set<std::string> &set,
                     const std::vector<std::string> &candidate) {
  return std::includes(set.begin(), set.end(), candidate.begin(),
                       candidate.end());
}

inline std::string setToString(const std::set<std::string> set) {
  std::string retval;
  for (const auto &s : set) {

    retval += ("'" + s + "' ");
  }

  return retval;
}

std::set<std::string>
waitForAny(std::map<std::string, std::future<bool>> &futuresMap,
           std::set<std::string> &finished, std::set<std::string> &running);

std::set<std::string> getDependingNodes(const std::set<std::string> &finished,
                                        const std::set<std::string> &queued,
                                        const DependencyMap &predecessorMap);

struct Timer {

  void start();
  long long stop();

private:
  std::chrono::steady_clock::time_point t1_;
  std::chrono::steady_clock::time_point t2_;
};

} // namespace helpers
} // namespace execution_nodes