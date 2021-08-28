#pragma once

#include <any>
#include <map>

#include <execution_nodes/connection.h>
#include <string>

namespace execution_nodes {

ConnectionId hash(const std::string &str);
typedef std::string PortId;

PortId createPortId(const std::string& nodeName, const std::string& portName);

class Connector {
public:
  Connector();

  void registerConnection(const std::string &nodeName,
                          const std::string &portName,
                          const std::string &connection);

  void setObject(ConnectionId connection, const std::any &obj);

  void getObject(ConnectionId connection, std::any &obj);

private:
  std::map<ConnectionId, std::any> objects_;

};

} // namespace execution_nodes