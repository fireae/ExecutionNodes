#pragma once

#include <any>
#include <map>

#include <execution_nodes/connection.h>
#include <string>
#include <memory>

namespace execution_nodes {

typedef std::string PortId;

class Connector {
public:
  Connector();

  void registerConnection(const std::string &nodeName,
                          const std::string &portName,
                          const std::string &connection);

  void setObject(const PortId &portId, const std::any &obj);
  void setObject(const PortId &portId, const std::any &&obj);
  bool hasObject(const PortId &portId);
  void getObject(const PortId &portId, std::any &obj);

  static ConnectionId hash(const std::string &str);

  static PortId createPortId(const std::string &nodeName,
                             const std::string &portName);

private:
  ConnectionId getConnectionByPortId(const PortId &portId);

  std::map<ConnectionId, std::any> objectsMap_;
  std::map<PortId, ConnectionId> connectionMap_;
};

typedef std::shared_ptr<Connector> ConnectorPtr;

} // namespace execution_nodes