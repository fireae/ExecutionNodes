#pragma once

#include <execution_nodes/connector.h>
#include <execution_nodes/node_definition.h>
#include <memory>

namespace execution_nodes {

class Node {
public:
  Node(const NodeDefinition &nodeDefinition,
       const std::shared_ptr<Connector> &connector);

  virtual void execute() = 0;

protected:

    template<class T>
    setOutput(const std::string& portName, const T& obj) {
        
    }




private:
  std::string name_;
  std::string type_;
  nlohmann::json settings_;
  std::map<std::string /*port name*/, ConnectionId> inputs_;
  std::map<std::string /*port name*/, ConnectionId> outputs_;
  std::shared_ptr<Connector> connector_;
  std::map<std::string /*port name*/, PortType> portTypeMap_;
};

} // namespace execution_nodes