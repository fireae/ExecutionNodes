#include <execution_nodes/internal/logging.hpp>
#include <execution_nodes/internal/topological_sort.h>

#include <map>
#include <set>

namespace execution_nodes {

// Helper function to check whether a node has a particular mark (either
// permanent or temporary), depending on the set specified.
inline bool hasMark(const std::string &nodeName,
                    const std::set<std::string> &marks) {
  return (marks.find(nodeName) != marks.end());
}
// Helper function to remove a mark from a node.
inline void removeMark(const std::string &nodeName,
                       std::set<std::string> &marks) {

  auto it = marks.find(nodeName);
  if (it != marks.end()) {
    marks.erase(it);
  }
}
// Helper function to add a mark to a node.
inline void addMark(const std::string &nodeName, std::set<std::string> &marks) {
  auto it = marks.find(nodeName);
  if (it == marks.end()) {
    marks.insert(nodeName);
  }
}

// This function returns a const iterator to the first node that has not a
// permanent mark. If there is no node without a permanent mark, cend() is
// returned.
DependencyMap::const_iterator
getUnmarkedNode(const DependencyMap &descMap,
                const std::set<std::string> permanentMarks) {

  DependencyMap::const_iterator it;

  for (it = descMap.cbegin(); it != descMap.cend(); it++) {
    const auto &nodeName = it->first;
    if (hasMark(nodeName, permanentMarks)) {
      continue;
    } else {
      return it;
    }
  }
  it = descMap.cend();
  return it;
}

// The visit function is called recursively and is at the core of the algorithm.
// It is called for each descendants of the node and marks the node along the
// way to find the last node at the end of the graph.
void visit(const std::string &nodeName, const DependencyMap &descMap,
           std::set<std::string> &permanentMarks,
           std::set<std::string> &temporaryMarks,
           std::list<std::string> &sortedList) {


  // If it has a permanent mark, we already added the node to the list.
  if (hasMark(nodeName, permanentMarks)) {
    // So we return in this case.
    return;
  }
  // If it already has a temporary mark, it means we already visited this node
  // along the way, so it depends on itself, so the graph is not acyclic. The
  // graph being acyclic is a necessary condition for successfull toplogical
  // sorting.
  if (hasMark(nodeName, temporaryMarks)) {
    THROW_ERROR
        << "The nodes in this graph contains at least one circular dependency. "
           "Please make sure the graph is acyclic.";
  }
  // Add temporary mark to the node. Temporary marks are here for discovering
  // cyclic dependencies.
  addMark(nodeName, temporaryMarks);
  // Find all the descendants of this node.
  auto it = descMap.find(nodeName);
  // only if there are descendants.
  if (it != descMap.end()) {
    // descendants holds the names of all the nodes which have an edge from this
    // node (nodeName) to them because this is how we built the descMap at the
    // beginning.
    const std::vector<std::string> &descendants = it->second;
    // For each node m with an edge from n to m:
    for (const auto &node : descendants) {
      visit(node, descMap, permanentMarks, temporaryMarks, sortedList);
    }
  }
  // If there are no descendants, we arrived at a node that should be executed
  // last compared to all other visited unmarked nodes.
  // So we remove the temporary mark, add a permanent mark (to never visit it
  // again).
  removeMark(nodeName, temporaryMarks);
  addMark(nodeName, permanentMarks);
  // Finally, add it to the beginning of the sorted list, pushing all  others
  // further back.
  sortedList.push_front(nodeName);
}

size_t countPredecessors(size_t counter, const std::string &nodeName,
                         const DependencyMap &predecMap) {

  auto it = predecMap.find(nodeName);
  if (it == predecMap.end()) {
    return 0u;
  }
  size_t maxPredecessors = 0;
  const auto &predecs = it->second;
  for (const auto &p : predecs) {
    size_t count = countPredecessors(counter, p, predecMap);
    count++;
    if (count > maxPredecessors) {
      maxPredecessors = count;
    }
  }
  return maxPredecessors;
}

/*
This algorithm is implemented according to Cormen, Thomas H.; Leiserson, Charles
E.; Rivest, Ronald L.; Stein, Clifford (2001), "Section 22.4: Topological sort",
Introduction to Algorithms (2nd ed.), MIT Press and McGraw-Hill, pp. 549–552,
ISBN 0-262-03293-7
*/
SortedNodes
getNodeExecutionOrder(const std::vector<ConnectionDefinition> edges) {

  // This empty list will contain the sorted nodes and be returned.
  // We use a std::list as container since it makes it easier to append to the
  // front.
  std::list<std::string> list;

  DependencyMap descMap;
  std::set<std::string /*node name*/> permanentMarks;
  std::set<std::string /*node name*/> temporaryMarks;

  // First build the descendancy map. This is used for lookup.
  for (const auto &edge : edges) {
    descMap[edge.src.nodeName].emplace_back(edge.dst.nodeName);
  }
  // The done flag indicates whether the algorithm is finished.
  // Done will be false as long as there exists at least one node without a
  // permanent mark.
  bool done = false;
  do {
    // Select an unmarked node.
    auto it = getUnmarkedNode(descMap, permanentMarks);

    if (it == descMap.cend()) {
      // if we are at the end of the map there is no unmarked node left.
      // Algorithm terminates.
      done = true;
    } else {
      // visit the node and all its descendants and mark them as we go.
      visit(it->first, descMap, permanentMarks, temporaryMarks, list);
    }

  } while (!done);

  DependencyMap predecMap;
  std::map<size_t, std::vector<std::string>> parallelMap;

  for (const auto &edge : edges) {
    predecMap[edge.dst.nodeName].emplace_back(edge.src.nodeName);
  }

  for (const auto &nodeName : list) {
    size_t n = countPredecessors(0, nodeName, predecMap);
    LOG_DEBUG << nodeName << " has " << n << " predecessors.";
    parallelMap[n].emplace_back(nodeName);
  }

  SortedNodes retval;

  for (const auto &e : list) {
    if (predecMap.find(e) == predecMap.end()) {
      predecMap[e] = {};
    }
  }

  // copy the list to a vector. I personally find a vector easier to work with,
  // that is why I want the function to return a vector rather than a list.
  retval.linearExecutionOrder =
      std::vector<std::string>(list.begin(), list.end());
  retval.parallelExecutionMap = parallelMap;
  retval.predecessorMap = predecMap;
  return retval;
}

} // namespace execution_nodes