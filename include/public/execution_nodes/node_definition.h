#pragma once

#include <nlohmann/json.hpp>

namespace execution_nodes {

/**
 * @brief This data structure defines name and type of a node as well as its
 * settings. The information in this structure is all which is necessary to
 * instantiate a new node.
 *
 */
struct NodeDefinition {
  /**
   * @brief The name of the node. This name must be unique within a given
   * graphen.
   */
  std::string name;
  /**
   * @brief The type of the node. This type name must be equal to the class name
   * of the node that shall be instantiated.
   */
  std::string type;
  /**
   * @brief The settings for this node. Settings are accessible in the
   * constructor of the node as well as in the execute() method.
   */
  nlohmann::json settings;

  NodeDefinition();
  NodeDefinition(const std::string nodeName, const std::string &nodeType,
                 nlohmann::json nodeSettings);
  NodeDefinition(const std::string nodeName, const std::string &nodeType);
};

/**
 * @brief Parse a @ref NodeDefinition object from a nlohmann::json
 *
 * @param j The json
 * @param d The node definition
 */
void from_json(const nlohmann::json &j, NodeDefinition &d);

} // namespace execution_nodes