#include <execution_nodes/internal/connector.h>
#include <execution_nodes/internal/logging.hpp>

#include <functional>

namespace execution_nodes {

Connector::Connector(){

};

void Connector::connect(const Port &out, const Port &in) {

  // In order to connect two ports we need to create a unique port id for both.
  auto outPortId = createPortId(out);
  auto inPortId = createPortId(in);

  // In case they have the same id we would be connecting the port to itself.
  // This is forbidden, hence runtime error is thrown.
  if (outPortId == inPortId) {
    LOG_ERROR
        << "Error when connecting ports: The port " << outPortId
        << " connects to itself. This is not allowed and will be ignored.";
    return;
  }
  auto iterOut = portTypeMap_.find(outPortId);
  if (iterOut == portTypeMap_.end()) {
    portTypeMap_[outPortId] = PortType::OUTPUT;
    outputPortsPerNode_[out.nodeName].insert(out.portName);
  } else if (iterOut->second == PortType::INPUT) {
    THROW_ERROR << "Error when connecting ports: The port '" << outPortId
                << "' was defined as input before but is now used as "
                   "output. Port can not be both.";
  }

  auto iterIn = portTypeMap_.find(inPortId);
  if (iterIn == portTypeMap_.end()) {
    portTypeMap_[inPortId] = PortType::INPUT;
    inputPortsPerNode_[in.nodeName].insert(in.portName);
  } else if (iterIn->second == PortType::OUTPUT) {
    THROW_ERROR << "Error when connecting ports: The port '" << inPortId
                << "' was defined as output before but is now used as "
                   "input. Port can not be both.";
  }

  registerOutput(out.nodeName, out.portName);
  registerAndConnectInput(in.nodeName, in.portName, outPortId);
}
void Connector::disconnect(const Port &out, const Port &in) {

  auto outPortId = createPortId(out);
  auto inPortId = createPortId(in);
  auto connectionName = createConnectionName(outPortId, inPortId);

  removeConnection(out.nodeName, out.portName, connectionName);
  removeConnection(in.nodeName, in.portName, connectionName);
}

void Connector::registerOutput(const std::string &nodeName,
                               const std::string &portName) {

  auto portId = createPortId(nodeName, portName);
  if (connectionMap_.find(portId) == connectionMap_.end()) {
    connectionMap_[portId] = portId;
  }
}

void Connector::registerAndConnectInput(const std::string &nodeName,
                                        const std::string &portName,
                                        const std::string &connection) {

  auto portId = createPortId(nodeName, portName);

  auto iter = connectionMap_.find(portId);
  if (iter == connectionMap_.end()) {
    connectionMap_[portId] = connection;
  } else {

    THROW_ERROR << "Error when connecting input port '" << portId << "' to '"
                << connection
                << "'. This input port is already "
                   "connected to '"
                << iter->second << "'";
  }
}

void Connector::removeConnection(const std::string &nodeName,
                                 const std::string &portName,
                                 const std::string &connection) {
  auto portId = createPortId(nodeName, portName);
  {
    auto iter = connectionMap_.find(portId);
    if (iter != connectionMap_.end()) {
      connectionMap_.erase(iter);
    } else {

      THROW_ERROR << "Error when disconnecting port with id '" << portId
                  << "' from '" << connection
                  << "'. This port has no connection";
    }
  }
  {
    auto iter = outputPortsPerNode_.find(nodeName);
    if (iter != outputPortsPerNode_.end()) {
      iter->second.erase(portName);
    }
  }
  {
    auto iter = inputPortsPerNode_.find(nodeName);
    if (iter != inputPortsPerNode_.end()) {
      iter->second.erase(portName);
    }
  }
}

ConnectionId Connector::getConnectionByPortId(const PortId &portId) {
  ConnectionId retval = no_connection;
  auto iter = connectionMap_.find(portId);
  if (iter != connectionMap_.end()) {
    retval = iter->second;
  }
  return retval;
}

void Connector::setObject(const PortId &portId, const std::any &obj) {

  auto iter = portTypeMap_.find(portId);

  if (iter == portTypeMap_.end()) {

    THROW_ERROR << "Error when setting output at port '" << portId
                << "' This port is undefined.";

  } else if (iter->second == PortType::OUTPUT) {
    auto connection = getConnectionByPortId(portId);
    if (connection != no_connection) {
      objectsMap_[connection] = obj;
    }
  } else {
    THROW_ERROR << "Error when setting output at port ' " << portId
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

    LOG_WARNING << "When calling hasObject(): The port " << portId
                << " is asking for input but it is not an input port.";

    retval = false;
  }
  return retval;
}

void Connector::getObject(const PortId &portId, std::any &obj) {

  auto iter = portTypeMap_.find(portId);
  if (iter == portTypeMap_.end()) {

    THROW_ERROR << "The undefined port '" << portId << "' is asking for input.";

  } else if (iter->second == PortType::INPUT) {

    auto connection = getConnectionByPortId(portId);

    if (connection != no_connection) {
      auto iter = objectsMap_.find(connection);
      if (iter != objectsMap_.end()) {
        obj = iter->second;
      } else {

        THROW_ERROR << "Error when getting object for port id '" << portId
                    << "'. Object does not exist.";
      }
    } else {

      THROW_ERROR << "Error when getting object for port id '" << portId
                  << "'. This port as no connection.";
    }

  } else {

    THROW_ERROR << "Error when getting object for port id " << portId
                << ". This port is not an input port.";
  }
}

void Connector::getObjectFromOutput(const PortId &portId, std::any &obj) {

      auto iter = portTypeMap_.find(portId);
  if (iter == portTypeMap_.end()) {

    THROW_ERROR << "The port port '" << portId << "' is undefined.";

  } else if (iter->second == PortType::OUTPUT) {

    auto connection = getConnectionByPortId(portId);

    if (connection != no_connection) {
      auto iter = objectsMap_.find(connection);
      if (iter != objectsMap_.end()) {
        obj = iter->second;
      } else {

        THROW_ERROR << "Error when getting object for port id '" << portId
                    << "'. Object does not exist.";
      }
    } else {

      THROW_ERROR << "Error when getting object for port id '" << portId
                  << "'. This port as no connection.";
    }

  } else {

    THROW_ERROR << "Error when getting object from id '" << portId
                << "'. This port is not an output port.";
  }

}

std::set<std::string> Connector::getConnectedPorts(const std::string &nodeName,
                                                   PortType type) {

  std::set<std::string> retval;

  if (type == PortType::INPUT) {
    auto iter = inputPortsPerNode_.find(nodeName);
    if (iter != inputPortsPerNode_.end()) {
      retval = iter->second;
    }
  } else if (type == PortType::OUTPUT) {
    auto iter = outputPortsPerNode_.find(nodeName);
    if (iter != outputPortsPerNode_.end()) {
      retval = iter->second;
    }
  } else {
    LOG_ERROR << "Unknown port type.";
  }
  return retval;
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