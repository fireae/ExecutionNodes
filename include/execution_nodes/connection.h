#pragma once
#include <cstdint>
#include <string>

namespace execution_nodes {

typedef std::string PortId;
typedef std::string ConnectionName;
typedef std::string ConnectionId;

static const std::string no_connection{""};

/**
 * @brief Defines a port.
 *
 */
struct Port {
  /**
   * @brief The name of the node this port belongs to.
   */
  std::string nodeName;
  /**
   * @brief The name of this port.
   */
  std::string portName;
  Port() = default;
  Port(const std::string &nodeName_, const std::string &portName_)
      : nodeName(nodeName_), portName(portName_) {}
};

/**
 * @brief Create a unique identifier from the port name and the node name.
 * The port id is simply a concatenation of the node name and the port name like
 * "node:port".
 *
 * @param port The port the id shall be generated for.
 * @return PortId The id of this port.
 */
PortId createPortId(const Port &port);

/**
 * @brief Create a unique identifier from the port name and the node name.
 * The port id is simply a concatenation of the node name and the port name like
 * "node:port".
 *
 * @param nodeName The name of the node.
 * @param portName The name of the port.
 * @return PortId The id if this port.
 */
PortId createPortId(const std::string &nodeName, const std::string &portName);

/**
 * @brief Create a unique name for a connection bewteen output port and input
 * port. The id is simply a concatenation of the out port id and the in port id
 * like "out -> in".
 *
 * @param out The id of output port.
 * @param in The id of the input port.
 * @return ConnectionName
 */
ConnectionName createConnectionName(const PortId &out, const PortId &in);

/**
 * @brief Helper data structure which defines a connection between two ports.
 */
struct ConnectionDefinition {
  /**
   * @brief The source of the connection.
   */
  Port src;
  /**
   * @brief The destination of the connection.
   */
  Port dst;
  ConnectionDefinition() = default;
  ConnectionDefinition(const Port &srcPort, const Port &dstPort)
      : src(srcPort), dst(dstPort) {}

  /**
   * @brief Comparison operator necessary for creating associative container
   * like std::set<ConnectionDefinition>.
   *
   * @param rhs The right hand side object.
   * @return true If smaller
   * @return false else
   */
  bool operator<(const ConnectionDefinition &rhs) const;
};

} // namespace execution_nodes