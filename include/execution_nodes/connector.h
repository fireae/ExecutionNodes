#pragma once

#include <any>
#include <map>

#include <execution_nodes/connection.h>
#include <string>
#include <memory>

namespace execution_nodes {

typedef std::string PortId;

enum class PortType : uint8_t {
  INPUT,
  OUTPUT,
};

struct Port{
    std::string nodeName;
    std::string portName;

    explicit Port(const std::string& nodeName_, const std::string& portName_) 
    :nodeName(nodeName_), portName(portName_)
    {
    }
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