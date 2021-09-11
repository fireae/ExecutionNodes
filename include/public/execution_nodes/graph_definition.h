#pragma once

#include <execution_nodes/connection.h>
#include <execution_nodes/node_definition.h>

namespace execution_nodes {

/**
 * @brief Data structure describing a graph and the nodes in it.
 *
 */
struct GraphDefinition {
  /**
   * @brief The name of this graph.
   */
  std::string name;
  /**
   * @brief A list of all nodes within this graph. The nodes can be specified in
   * any order.
   */
  std::vector<NodeDefinition> nodes;
  /**
   * @brief A list specifying all connection between nodes in the graph.
   *
   */
  std::vector<ConnectionDefinition> connections;
};

/**
 * @brief Load a graph definition from a json file.
 *
 * @param jsonFilePath The path to the json file.
 * @return GraphDefinition The resulting graph definition.
 */
GraphDefinition loadGraphDefFromJsonFile(const std::string &jsonFilePath);
/**
 * @brief Load a graph definition from a json object.
 *
 * @param j The json object.
 * @return GraphDefinition The resulting graph definition.
 */
GraphDefinition loadGraphDefFromJson(const nlohmann::json &j);

} // namespace execution_nodes