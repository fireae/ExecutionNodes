#pragma once

#include <execution_nodes/connector.h>
#include <execution_nodes/logging.hpp>


#include <string>

#include <functional>

namespace execution_nodes {



Connector::Connector(){

};

void Connector::connect(const Port &out, const Port &in) {

  auto outPortId = createPortId(out);
  auto inPortId = createPortId(in);
  auto connectionName = createConnectionName(outPortId, inPortId);

  registerConnection(out.nodeName, out.portName, connectionName);
  registerConnection(in.nodeName, in.portName, connectionName);

  auto iterOut = portTypeMap_.find(outPortId);

  if (iterOut == portTypeMap_.end()) {
    portTypeMap_[outPortId] = PortType::OUTPUT;
  } else if (iterOut->second == PortType::INPUT) {
    Log().ErrorThrow() << "The port '" << outPortId
                       << "' was defined as input before but is now used as "
                          "output. Port can not be both.";
  }

  auto iterIn = portTypeMap_.find(inPortId);
  if (iterIn == portTypeMap_.end()) {
    portTypeMap_[inPortId] = PortType::INPUT;
  } else if (iterIn->second == PortType::OUTPUT) {
    Log().ErrorThrow() << "The port '" << inPortId
                       << "' was defined as output before but is now used as "
                          "input. Port can not be both.";
  }
}
void Connector::disconnect(const Port &out, const Port &in) {

  auto outPortId = createPortId(out);
  auto inPortId = createPortId(in);
  auto connectionName = createConnectionName(outPortId, inPortId);

  removeConnection(out.nodeName, out.portName, connectionName);
  removeConnection(in.nodeName, in.portName, connectionName);
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

    Log().ErrorThrow() << "Error when connecting port with id '" << portId
                       << "' to '" << connection
                       << "'. This port is already "
                          "connected to '"
                       << iter->second << "'";
  }
}

void Connector::removeConnection(const std::string &nodeName,
                                 const std::string &portName,
                                 const std::string &connection) {
  auto portId = createPortId(nodeName, portName);

  auto iter = connectionMap_.find(portId);
  if (iter != connectionMap_.end()) {
    connectionMap_.erase(iter);
  } else {

    Log().ErrorThrow() << "Error when disconnecting port with id '" << portId
                       << "' from '" << connection
                       << "'. This port has no connection";
  }
}

ConnectionId Connector::getConnectionByPortId(const PortId &portId) {
  ConnectionId retval = no_connection;
  auto iter = connectionMap_.find(portId);
  if (iter == connectionMap_.end()) {
    Log().ErrorThrow() << "Error when getting connection for port id '"
                       << portId << "'. This port has no connection.";
  } else {
    retval = iter->second;
  }
  return retval;
}

void Connector::setObject(const PortId &portId, const std::any &obj) {

  auto iter = portTypeMap_.find(portId);

  if (iter == portTypeMap_.end()) {

    Log().ErrorThrow() << "Error when setting output at port ' " << portId
                       << "This port is undefined.";

  } else if (iter->second == PortType::OUTPUT) {
    auto connection = getConnectionByPortId(portId);
    if (connection != no_connection) {
      objectsMap_[connection] = obj;
    }
  } else {
    Log().ErrorThrow() << "Error when setting output at port ' " << portId
                       << "This port is not an output port.";
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

    Log().Warning() << "The port " << portId
                    << " is asking for input but it is not an input port.";

    retval = false;
  }
  return retval;
}

void Connector::getObject(const PortId &portId, std::any &obj) {

  auto iter = portTypeMap_.find(portId);
  if (iter == portTypeMap_.end()) {

    Log().ErrorThrow() << "The undefined port '" << portId
                       << "' is asking for input.";

  } else if (iter->second == PortType::INPUT) {

    auto connection = getConnectionByPortId(portId);

    if (connection != no_connection) {
      auto iter = objectsMap_.find(connection);
      if (iter != objectsMap_.end()) {
        obj = iter->second;
      } else {

        Log().ErrorThrow() << "Error when getting object for port id '"
                           << portId << "'. Object does not exist.";
      }
    } else {

      Log().ErrorThrow() << "Error when getting object for port id '" << portId
                         << "'. This port as no connection.";
    }

  } else {

    Log().ErrorThrow() << "Error when getting object for port id " << portId
                       << ". This port is not an input port.";
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