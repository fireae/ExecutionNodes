#pragma once

#include <execution_nodes/graph_definition.h>
#include <execution_nodes/node.h>
#include <execution_nodes/node_registry.h>

namespace execution_nodes {

class Graph {

public:
  explicit Graph(const GraphDefinition &graphDefinition,
                 const NodeRegistry &registry);

  void execute();

  std::string name_;
  std::shared_ptr<Connector> connector_;
  NodeRegistry registry_;
  std::vector<std::unique_ptr<Node>> nodes_;
};

} // namespace execution_nodes