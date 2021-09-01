#pragma once

#include <execution_nodes/node.h>

namespace execution_nodes {

class Graph {

public:
  Graph();

  void execute();

  std::vector<std::unique_ptr<Node>> nodes_;
};

} // namespace execution_nodes