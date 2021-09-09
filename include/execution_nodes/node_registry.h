#pragma once

#include <execution_nodes/connector.h>
#include <execution_nodes/node.h>
#include <execution_nodes/node_definition.h>
#include <functional>

namespace execution_nodes {

typedef std::function<NodePtr(const NodeDefinition &,
                              const std::shared_ptr<Connector> &)>
    NodeCreateFunction;

/**
 * @brief The NodeRegistry is an associative container that stores a node create
 * function function to a map. This way it is possible to instantiate a concrete
 * node using its class name.
 */
typedef std::map<std::string /*node name*/, NodeCreateFunction> NodeRegistry;

/**
 * @brief Macro that removed boiler plate when registering a new node in the
 * registry.
 *
 */
#define REGISTER(nodeTypeName)                                                 \
  { #nodeTypeName, execution_nodes::createNode < nodeTypeName> }

} // namespace execution_nodes
