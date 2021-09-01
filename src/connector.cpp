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

PortId Connector::createPortId(const std::string &nodeName,
                               const std::string &portName) {
  return nodeName + ":" + portName;
}

Connector::Connector(){

};

void Connector::registerConnection(const std::string &nodeName,
                                   const std::string &portName,
                                   const std::string &connection) {

  auto portId = createPortId(nodeName, portName);
  auto connectionId = hash(connection);

  auto iter = connectionMap_.find(portId);
  if (iter == connectionMap_.end()) {
    connectionMap_[portId] = connectionId;
  } else {
    Log::throwError(
        "Error when connecting port with id '{}' to '{}'. This port is already "
        "connected to '{}'",
        portId, connection, iter->second);
  }
}

ConnectionId Connector::getConnectionByPortId(const PortId &portId) {
  ConnectionId retval = no_connection;
  auto iter = connectionMap_.find(portId);
  if (iter == connectionMap_.end()) {
    Log::throwError("Error when getting connection for port id '{}'. This port has no connection.", portId);
  } else {
    retval = iter->second;
  }
  return retval;
}

void Connector::setObject(const PortId &portId, const std::any &obj) {
  auto connection = getConnectionByPortId(portId);
  if (connection != no_connection) {
    objectsMap_[connection] = obj;
  }
}

void Connector::setObject(const PortId &portId, const std::any &&obj) {

  auto connection = getConnectionByPortId(portId);
  if (connection != no_connection) {
    objectsMap_[connection] = std::move(obj);
  }
}

bool Connector::hasObject(const PortId &portId) {
  auto connection = getConnectionByPortId(portId);
  if (connection != no_connection) {
    return objectsMap_.find(connection) != objectsMap_.end();
  } else {
    return false;
  }
}

void Connector::getObject(const PortId &portId, std::any &obj) {

  auto connection = getConnectionByPortId(portId);

  if (connection != no_connection) {
    auto iter = objectsMap_.find(connection);
    if (iter != objectsMap_.end()) {
      obj = iter->second;
    } else {
      Log::throwError(
          "Error when getting object for port id '{}'. Object does not "
            "exist.",
            portId);
    }
  } else {
    Log::throwError(
        "Error when getting object for port id '{}'. Port has no connection.");
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