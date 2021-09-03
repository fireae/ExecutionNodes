#include <execution_nodes/logging.hpp>
#include <execution_nodes/topological_sort.h>

#include <map>
#include <set>

namespace execution_nodes {

inline bool hasMark(const std::string &nodeName,
                    const std::set<std::string> &marks) {
  return (marks.find(nodeName) != marks.end());
}
inline void removeMark(const std::string &nodeName,
                       std::set<std::string> &marks) {

  auto it = marks.find(nodeName);
  if (it != marks.end()) {
    marks.erase(it);
  }
}
inline void addMark(const std::string &nodeName, std::set<std::string> &marks) {
  auto it = marks.find(nodeName);
  if (it == marks.end()) {
    marks.insert(nodeName);
  }
}

typedef std::map<std::string /*node name*/,
                 std::vector<std::string> /*node names*/>
    AdjacencyMap;

AdjacencyMap::const_iterator
getUnmarkedNode(const AdjacencyMap &adjMap,
                const std::set<std::string> permanentMarks) {

  AdjacencyMap::const_iterator it;

  for (it = adjMap.cbegin(); it != adjMap.cend(); it++) {
    const auto &nodeName = it->first;
    if (hasMark(nodeName, permanentMarks)) {
      continue;
    } else {
      return it;
    }
  }
  it = adjMap.cend();
  return it;
}

void visit(const std::string &nodeName, const AdjacencyMap &adjMap,
           std::set<std::string> &permanentMarks,
           std::set<std::string> &temporaryMarks,
           std::list<std::string> &sortedList) {

  if (hasMark(nodeName, permanentMarks)) {
    return;
  }
  if (hasMark(nodeName, temporaryMarks)) {
    Log().ErrorThrow()
        << "The nodes in this graph contain at least one circular dependency. "
           "Please make sure the graph is acyclic.";
  }
  addMark(nodeName, temporaryMarks);

  auto it = adjMap.find(nodeName);
  if (it != adjMap.end()) {
    const std::vector<std::string> &adjacentNodes = it->second;

    for (const auto &node : adjacentNodes) {
      visit(node, adjMap, permanentMarks, temporaryMarks, sortedList);
    }
  }
  removeMark(nodeName, temporaryMarks);
  addMark(nodeName, permanentMarks);
  sortedList.push_front(nodeName);
}

std::vector<std::string /*node name*/>
getNodeExecutionOrder(const std::vector<ConnectionDefinition> edges) {

  std::list<std::string> list;

  AdjacencyMap adjMap;
  std::set<std::string /*node name*/> permanentMarks;
  std::set<std::string /*node name*/> temporaryMarks;

  for (const auto &edge : edges) {
    adjMap[edge.src.nodeName].emplace_back(edge.dst.nodeName);
  }

  bool done = false;
  do {
    auto it = getUnmarkedNode(adjMap, permanentMarks);

    if (it == adjMap.cend()) {
      done = true;
    } else {
      visit(it->first, adjMap, permanentMarks, temporaryMarks, list);
    }

  } while (!done);

  std::vector<std::string> retval(list.begin(), list.end());
  return retval;
}

} // namespace execution_nodes