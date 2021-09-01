#pragma once

#include <execution_nodes/connector.h>
#include <execution_nodes/node_definition.h>
#include <memory>
#include <vector>
#include <execution_nodes/logging.hpp>

namespace execution_nodes {



class Node {
public:
  Node(const NodeDefinition &nodeDefinition,
       const std::shared_ptr<Connector> &connector);
  virtual void execute() = 0;

  std::string getName();
  std::string getType();
  std::vector<std::string> getInputPortNames();
  std::vector<std::string> getOutputPortNames();

protected:
  template <class T> void setOutput(const std::string &portName, const T &obj) {

    auto iter = portTypeMap_.find(portName);

    if (iter == portTypeMap_.end()) {
      Log::throwError("Error when setting output at port '{}' in node '{}' of type '{}': "
            "This port is undefined.",
            portName, name_, type_);
    } else if (iter->second == PortType::OUTPUT) {
      connector_->setObject(Connector::createPortId(name_, portName), obj);
    } else {
      Log::throwError(
          "Error when setting output port '{}' in node '{}' of type '{}': "
            "This port id not defined as output.",
            portName, name_, type_)
    }
  }
  bool hasInput(const std::string &portName);
  template <class T> void getInput(const std::string &portName, const T &obj) {

    auto iter = portTypeMap_.find(portName);
    if (iter == portTypeMap_.end()) {
      Log::throwError(
          "Node '{}' of type '{}' is asking for input on port '{}'. "
            "This port is not defined.",
            name_, type_, portName);
    } else if (iter->second == PortType::INPUT) {
      try {
        std::any anyObj;
        connector_->getObject(Connector::createPortId(name_, portName), anyObj);
        obj = std::any_cast<T>(anyObj);
      } catch (const std::bad_any_cast &bac) {
        Log::throwError(
            "Error in node '{}' of type '{}' when getting object from input "
              "on port '{}'. This error might come "
              "from a wrong typename in the template argument. Additional "
              "information: {}",
              name_, type_, portName, bac.what());
      } catch (const std::exception &ex) {
        Log::throwError(
            "Error in node '{}' of type '{}' when getting object from input "
              "on port '{}'. Additional information: {}",
              name_, type_, portName, ex.what());
      }

    } else {
      Log::throwError("Node '{}' of type '{}' is asking for input on port '{}'."
            "This port is not defined as input port",
            name_, type_, portName);
    }
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


template<class DerivedNodeType>
std::unique_ptr<Node> createNode(const NodeDefinition &nodeDefinition,
                                 const std::shared_ptr<Connector> &connector) {
    return std::make_unique<Node>(DerivedNodeType(nodeDefinition, connector));
}

} // namespace execution_nodes