#include <execution_nodes/internal/connector.h>
#include <execution_nodes/internal/logging.hpp>
#include <execution_nodes/node.h>

namespace execution_nodes {

Node::Node(const NodeDefinition &nodeDefinition, const ConnectorPtr &connector)
    : name_(nodeDefinition.name), type_(nodeDefinition.type),
      settings_(nodeDefinition.settings), connector_(connector) {}

bool Node::hasInput(const std::string &portName) {
  return connector_->hasObject(createPortId(name_, portName));
}

std::set<std::string> Node::getInputPortNames() {
  return getPortNames(PortType::INPUT);
}
std::set<std::string> Node::getOutputPortNames() {
  return getPortNames(PortType::OUTPUT);
}
std::set<std::string> Node::getPortNames(PortType type) {
  return connector_->getConnectedPorts(name_, type);
}

void Node::setOutputInternal(const std::string &portName,
                             const std::any &anyObj) {
  connector_->setObject(createPortId(name_, portName), anyObj);
}

void Node::getInputInternal(const std::string &portName, std::any &anyObj) {

  connector_->getObject(createPortId(name_, portName), anyObj);
}

std::string Node::getName() { return name_; }
std::string Node::getType() { return type_; }

} // namespace execution_nodes