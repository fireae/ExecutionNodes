#pragma once

#include <any>
#include <map>

#include <execution_nodes/connection.h>
#include <memory>
#include <string>

namespace execution_nodes {

typedef std::string PortId;
typedef std::string ConnectionName;

enum class PortType : uint8_t {
  INPUT,
  OUTPUT,
};



class Connector {
public:
  Connector();

  void connect(const Port &out, const Port &in);

  void setObject(const PortId &portId, const std::any &obj);
  bool hasObject(const PortId &portId);
  void getObject(const PortId &portId, std::any &obj);

  static ConnectionId hash(const std::string &str);

  static PortId createPortId(const std::string &nodeName,
                             const std::string &portName);

  PortId Connector::createPortId(const Port &port);

private:
  ConnectionId getConnectionByPortId(const PortId &portId);

  std::map<ConnectionId, std::any> objectsMap_;
  std::map<PortId, ConnectionId> connectionMap_;
  std::map<PortId, PortType> portTypeMap_;

  void registerConnection(const std::string &nodeName,
                          const std::string &portName,
                          const std::string &connection);
};

typedef std::shared_ptr<Connector> ConnectorPtr;

} // namespace execution_nodes