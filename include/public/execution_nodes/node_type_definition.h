#pragma once

#include <execution_nodes/port_type_definition.h>
#include <nlohmann/json.hpp>

namespace execution_nodes {

/**
 * @brief Defines a node type. This is used as exchange information to the node
 * editor or other systems.
 *
 */
struct NodeTypeDefinition {
  /**
   * @brief The name of the node type, identical with the class name of the node
   *
   */
  std::string node_type;
  /**
   * @brief The input ports for this type of  node
   *
   */
  std::vector<PortTypeDefinition> input_ports;
  /**
   * @brief The output ports for this type of node
   *
   */
  std::vector<PortTypeDefinition> output_ports;
  /**
   * @brief The settings with default values for this type of node
   *
   */
  nlohmann::json default_settings;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(NodeTypeDefinition, //
                                   node_type,          //
                                   input_ports,        //
                                   output_ports,       //
                                   default_settings);

} // namespace execution_nodes