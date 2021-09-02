#pragma once
#include <execution_nodes/logging.hpp>
#include <execution_nodes/node.h>

namespace execution_nodes {

Node::Node(const NodeDefinition &nodeDefinition,
           const std::shared_ptr<Connector> &connector)
    : name_(nodeDefinition.name), type_(nodeDefinition.type),
      settings_(nodeDefinition.settings), connector_(connector) {
}

bool Node::hasInput(const std::string &portName) {
  return connector_->hasObject(Connector::createPortId(name_, portName));
}

std::string Node::getName() { return name_; }
std::string Node::getType() { return type_; }

} // namespace execution_nodes