#pragma once

#include <execution_nodes/graph_definition.h>
#include <execution_nodes/node.h>
#include <execution_nodes/node_registry.h>
#include <set>

namespace execution_nodes {

class Graph {

public:
  explicit Graph(const GraphDefinition &graphDefinition,
                 const NodeRegistry &registry);

  void execute();

  void addConnection(ConnectionDefinition connection, bool reorderNodes = true);
  void removeConnection(ConnectionDefinition connection,
                        bool reorderNodes = true);

private:
  std::string name_;
  std::shared_ptr<Connector> connector_;
  NodeRegistry registry_;
  std::vector<std::unique_ptr<Node>> nodes_;
  std::set<ConnectionDefinition> connections_;
  void sortNodes();

  void checkIfConnectionIsValid(const ConnectionDefinition &connection);
};

} // namespace execution_nodes