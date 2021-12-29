#pragma once

#include <nlohmann/json.hpp>

namespace execution_nodes {
/**
 * @brief Defines a port type. This is used as exchange information to the node
 * editor or other systems.
 */
struct PortTypeDefinition {
  /**
   * @brief The name of the port
   *
   */
  std::string port_name;
  /**
   * @brief The name of the data type transported via this port
   *
   */
  std::string data_type;
  /**
   * @brief Whether the input port is optional or required. This field is only
   * applicable for input ports. For output ports this value can be ignored.
   *
   */
  bool optional;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PortTypeDefinition, //
                                   port_name,          //
                                   data_type,          //
                                   optional);

} // namespace execution_nodes