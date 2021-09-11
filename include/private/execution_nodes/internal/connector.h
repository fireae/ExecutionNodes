#pragma once

#include <any>
#include <map>
#include <set>

#include <execution_nodes/connection.h>
#include <memory>
#include <mutex>
#include <string>

namespace execution_nodes {

/**
 * @brief The function of the Connector class is to hold the objects which are
 * exchanged between connected nodes and associates them with a connection. It
 * also stores all the connection information, meaning it knows which output
 * port from which node is allowed to transfer objects to which input port(s) of
 * which node(s).
 */
class Connector {
public:
  Connector();
  // Connector(const Connector &) = delete;

  /**
   * @brief Connect an output port with an input port.
   *
   * @param out The output port.
   * @param in The input port.
   */
  void connect(const Port &out, const Port &in);
  /**
   * @brief Disconnect input from output port.
   *
   * @param out The output port.
   * @param in The input port.
   */
  void disconnect(const Port &out, const Port &in);

  /**
   * @brief Set an object on a specified port. The port must be an output port.
   * Provided there exists a connection from this output port, the object will
   * be stored in an container that associates it with that connection. This way
   * each connected input port can have access to the object.
   * @param portId The id of the port.
   * @param obj The object to be set. Can be any type.
   */
  void setObject(const PortId &portId, const std::any &obj);
  /**
   * @brief Checks whether the specified input port has an object available at
   * its associated connection.
   *
   * @param portId The id of the input port.
   * @return true If there is an object available. Get it with @ref getObject().
   * @return false If there is no object available.
   */
  bool hasObject(const PortId &portId);
  /**
   * @brief Get the Object object for the specified input port.
   *
   * @param portId The id of the port.
   * @param obj The object to be retrieved. Can be any type.
   */
  void getObject(const PortId &portId, std::any &obj);

  /**
   * @brief Get the object from the specified output port. This function would
   * never be called by any node. This is because any given node should not have
   * any information about its connected output ports that feed into the input
   * ports.
   *
   * @param portId The id of the port.
   * @param obj The object to be retrieved. Can be any type.
   */
  void getObjectFromOutput(const PortId &portId, std::any &obj);

  /**
   * @brief Get a list of all connected ports of specified port type for a given
   * node name.
   *
   * @param nodeName The name of the node.
   * @param type The type of the node.
   * @return std::set<std::string> The list of all port names that satisfy the
   * specified criteria.
   */
  std::set<std::string> getConnectedPorts(const std::string &nodeName,
                                          PortType type);

private:
  Connector(const Connector&) = delete;

  std::mutex mutex_;


  // this map holds any object for each connection. Each connection can only
  // hold one object or no object at all, meaning there is no entry in this map
  // for that connection.
  std::map<ConnectionId, std::any> objectsMap_;
  // this map stores the connection id for each port id.
  std::map<PortId, ConnectionId> connectionMap_;
  // this map stores the type of the port for each port id.
  std::map<PortId, PortType> portTypeMap_;
  // This map stores all port names of type INPUT for each node name.
  std::map<std::string /*node name*/, std::set<std::string /*port name*/>>
      inputPortsPerNode_;
  // This map stores all port names of type OUTPUT for each node name.
  std::map<std::string /*node name*/, std::set<std::string /*port name*/>>
      outputPortsPerNode_;

  /**
   * @brief Register a new output port. This sets the necessary entries in the
   * maps. The connector then knows that the port with name portName belongs to
   * node with name nodeName and that it is an output.
   * @param nodeName The name of the node.
   * @param portName The name of the port.
   */
  void registerOutput(const std::string &nodeName, const std::string &portName);

  /**
   * @brief Register a new input and connect it. This sets the necessary entrie
   * sin the maps. The connector then knows that the port with name portName is
   * an input and that it belongs to the node with name nodeName and also that
   * it is connected to the specified connection.
   *
   * @param nodeName The name of the node.
   * @param portName The name of the port.
   * @param connection The connection
   */
  void registerAndConnectInput(const std::string &nodeName,
                               const std::string &portName,
                               const std::string &connection);

  /**
   * @brief Remove a connection.
   *
   * @param nodeName
   * @param portName
   * @param connection
   */
  void removeConnection(const std::string &nodeName,
                        const std::string &portName,
                        const std::string &connection);
};

typedef std::shared_ptr<Connector> ConnectorPtr;

} // namespace execution_nodes