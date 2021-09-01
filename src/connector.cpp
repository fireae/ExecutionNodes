#pragma once

#include <execution_nodes/connector.h>
#include <execution_nodes/logging.hpp>

#include <string>

#include <functional>

namespace execution_nodes {

static const std::hash<std::string> stringHasher;

ConnectionId Connector::hash(const std::string &str) {
  return static_cast<ConnectionId>(stringHasher(str));
}

PortId Connector::createPortId(const Port &port) {
  return port.nodeName + ":" + port.portName;
}

PortId Connector::createPortId(const std::string &nodeName,
                               const std::string &portName) {
  return nodeName + ":" + portName;
}

Connector::Connector(){

};

void Connector::connect(const Port &out, const Port &in) {

  const std::string connection = createPortId(out.nodeName, out.portName);
  registerConnection(out.nodeName, out.portName, connection);
  registerConnection(in.nodeName, in.portName, connection);

  portTypeMap_[createPortId(out)] = PortType::OUTPUT;
  portTypeMap_[createPortId(in)] = PortType::INPUT;
}

void Connector::registerConnection(const std::string &nodeName,
                                   const std::string &portName,
                                   const std::string &connection) {

  auto portId = createPortId(nodeName, portName);
  auto connectionId = hash(connection);

  auto iter = connectionMap_.find(portId);
  if (iter == connectionMap_.end()) {
    connectionMap_[portId] = connectionId;
  } else {
    /*
        "Error when connecting port with id '{}' to '{}'. This port is already "
        "connected to '{}'",
        portId, connection, iter->second);
      Log::throwError(
    */
  }
}

ConnectionId Connector::getConnectionByPortId(const PortId &portId) {
  ConnectionId retval = no_connection;
  auto iter = connectionMap_.find(portId);
  if (iter == connectionMap_.end()) {
    // Log::throwError("Error when getting connection for port id '{}'. This
    // port has no connection.", portId);
  } else {
    retval = iter->second;
  }
  return retval;
}

void Connector::setObject(const PortId &portId, const std::any &obj) {

  auto iter = portTypeMap_.find(portId);

  if (iter == portTypeMap_.end()) {
    /*
      Log::throwError("Error when setting output at port '", portName,
                    "' in node '", name_, "' of type '", type_,
                    "': "
                    "This port is undefined.");
    */
  } else if (iter->second == PortType::OUTPUT) {
    auto connection = getConnectionByPortId(portId);
    if (connection != no_connection) {
      objectsMap_[connection] = obj;
    }
  } else {
  }
}

bool Connector::hasObject(const PortId &portId) {

  bool retval = false;
  auto iter = portTypeMap_.find(portId);
  if (iter == portTypeMap_.end()) {
    retval = false;
  } else if (iter->second == PortType::INPUT) {
    auto connection = getConnectionByPortId(portId);
    if (connection != no_connection) {
      retval = objectsMap_.find(connection) != objectsMap_.end();
    } else {
      retval = false;
    }
  } else {
    /*
      Log::warning("Node '{}' of type '{}' is asking for input on port '{}' "
                 "which is not an input port",
                 name_, type_, portName);
    */
    retval = false;
  }
  return retval;
}

void Connector::getObject(const PortId &portId, std::any &obj) {

  auto iter = portTypeMap_.find(portId);
  if (iter == portTypeMap_.end()) {
    /*
      Log::throwError("Node '", name_, "' of type '", type_,
                    "' is asking for input on port '", portName,
                    "'. "
                    "This port is not defined.");
    */
  } else if (iter->second == PortType::INPUT) {

    auto connection = getConnectionByPortId(portId);

    if (connection != no_connection) {
      auto iter = objectsMap_.find(connection);
      if (iter != objectsMap_.end()) {
        obj = iter->second;
      } else {
        /*
          Log::throwError(
            "Error when getting object for port id '{}'. Object does not "
              "exist.",
              portId);
        */
      }
    } else {
      /*
        Log::throwError(
          "Error when getting object for port id '{}'. Port has no
        connection.");
      */
    }

  } else {
    /*
       Log::throwError("Node '", name_, "' of type '", type_,
                     "' is asking for input on port '", portName,
                     "'."
                     "This port is not defined as input port");
    */
  }
}
/*
const std::any &Connector::getObject(ConnectionId connection) {

  auto iter = objectsMap_.find(connection);
  if (iter != objectsMap_.end()) {
        return iter->second;
  } else {
    error("Error when getting object for connection id {}. Object does not "
          "exist.",
          connection);
  }
}
*/

} // namespace execution_nodes