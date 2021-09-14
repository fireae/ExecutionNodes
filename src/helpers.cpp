#include <execution_nodes/internal/helpers.h>
#include <execution_nodes/internal/logging.h>
#include <sstream>

namespace execution_nodes {

namespace helpers {
std::vector<std::string> split(const std::string &str, char delimiter) {
  std::string line;
  std::vector<std::string> tokens;
  std::stringstream ss(str);
  while (std::getline(ss, line, delimiter)) {
    tokens.push_back(line);
  }
  return tokens;
}

size_t getIndexOfElement(const std::string &element,
                         const std::vector<std::string> &list, bool doThrow) {
  for (size_t i = 0; i < list.size(); i++) {
    if (list[i] == element) {
      return i;
    }
  }
  if (doThrow) {
    THROW_ERROR << "Error when sorting nodes. Unable to find node '" << element
                << "' in the list of connections";
  }
  return list.size() + 1;
}

std::set<std::string>
waitForAny(std::map<std::string, std::future<bool>> &futuresMap,
           std::set<std::string> &finished, std::set<std::string> &running) {
  std::set<std::string> done = {};

  bool atLeastOneDone = false;
  static const std::chrono::duration d = std::chrono::microseconds(10);
  while (atLeastOneDone == false) {

    auto iter = futuresMap.begin();

    do {

      auto status = iter->second.wait_for(d);
      if (status == std::future_status::ready) {
        finished.insert(iter->first);
        running.erase(iter->first);
        done.insert(iter->first);
        iter = futuresMap.erase(iter);
        atLeastOneDone = true;
      } else {
        iter++;
      }

    } while (iter != futuresMap.end());
  }
  return done;
}

std::set<std::string> getDependingNodes(const std::set<std::string> &finished,
                                        const std::set<std::string> &queued,
                                        const DependencyMap &predecessorMap) {
  std::set<std::string> retval = {};

  for (const auto &q : queued) {
    auto predecessorIter = predecessorMap.find(q);
    if (predecessorIter != predecessorMap.end()) {
      const auto &predecessors = predecessorIter->second;
      if (helpers::isSubset(finished, predecessors)) {
        retval.insert(q);
      }
    }
  }
  return retval;
}

void Timer::start() { t1_ = std::chrono::high_resolution_clock::now(); }

long long Timer::stop() {

  t2_ = std::chrono::high_resolution_clock::now();
  // Getting number of milliseconds as an integer.
  auto ms_int =
      std::chrono::duration_cast<std::chrono::milliseconds>(t2_ - t1_);
  return ms_int.count();
}

} // namespace helpers
} // namespace execution_nodes