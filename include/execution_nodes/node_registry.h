#pragma once

#include <execution_nodes/connector.h>
#include <execution_nodes/node.h>
#include <execution_nodes/node_definition.h>
#include <functional>

namespace execution_nodes {

typedef std::function<NodePtr(const NodeDefinition &,
                              const std::shared_ptr<Connector> &)>
    NodeCreateFunction;

typedef std::map<std::string /*node name*/, NodeCreateFunction> NodeRegistry;

#define REGISTER(nodeTypeName)                                                 \
  { #nodeTypeName, execution_nodes::createNode<nodeTypeName> }                                  

} // namespace execution_nodes
