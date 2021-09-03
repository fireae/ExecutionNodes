#pragma once

#include <any>
#include <map>

#include <execution_nodes/connection.h>
#include <memory>
#include <string>

namespace execution_nodes {

enum class PortType : uint8_t {
  INPUT,
  OUTPUT,
};

class Connector {
public:
  Connector();

  void connect(const Port &out, const Port &in);
  void disconnect(const Port &out, const Port &in);

  void setObject(const PortId &portId, const std::any &obj);
  bool hasObject(const PortId &portId);
  void getObject(const PortId &portId, std::any &obj);

private:
  ConnectionId getConnectionByPortId(const PortId &portId);

  std::map<ConnectionId, std::any> objectsMap_;
  std::map<PortId, ConnectionId> connectionMap_;
  std::map<PortId, PortType> portTypeMap_;

  void Connector::registerOutput(const std::string &nodeName,
                                 const std::string &portName);

  void registerAndConnectInput(const std::string &nodeName,
                               const std::string &portName,
                               const std::string &connection);

  void removeConnection(const std::string &nodeName,
                        const std::string &portName,
                        const std::string &connection);
};

typedef std::shared_ptr<Connector> ConnectorPtr;

} // namespace execution_nodes