#pragma once
#include <execution_nodes/node.h>
#include <execution_nodes/logging.hpp>

namespace execution_nodes {

Node::Node(const NodeDefinition &nodeDefinition,
           const std::shared_ptr<Connector> &connector)
    : name_(nodeDefinition.name), type_(nodeDefinition.type),
      settings_(nodeDefinition.settings), connector_(connector) {

  for (const auto &i : nodeDefinition.inputs) {
    inputs_[i.first] = Connector::hash(i.second);
    portTypeMap_[i.first] = PortType::INPUT;
  }

  for (const auto &o : nodeDefinition.outputs) {

    if (portTypeMap_.find(o.first) != portTypeMap_.end()) {

      Log::throwError("The port name {} is already defined as input port. Port "
                    "names must be unique per node.",
                    o.first);
    } else {
      outputs_[o.first] = Connector::hash(o.second);
      portTypeMap_[o.first] = PortType::OUTPUT;
    }
  }
}

bool Node::hasInput(const std::string &portName) {
  bool retval = false;
  auto iter = portTypeMap_.find(portName);
  if (iter == portTypeMap_.end()) {
    retval = false;
  } else if (iter->second == PortType::INPUT) {
    retval = connector_->hasObject(Connector::createPortId(name_, portName));
  } else {
    Log::warning("Node '{}' of type '{}' is asking for input on port '{}' "
                 "which is not an input port",
                 name_, type_, portName);
    retval = false;
  }
  return retval;
}

std::string Node::getName() { return name_; }
std::string Node::getType() { return type_; }
std::vector<std::string> Node::getInputPortNames() {

  std::vector<std::string> retval;
  for (const auto &kvp : inputs_) {
    retval.emplace_back(kvp.first);
  }
  return retval;
}
std::vector<std::string> Node::getOutputPortNames() {
  std::vector<std::string> retval;
  for (const auto &kvp : outputs_) {
    retval.emplace_back(kvp.first);
  }
  return retval;
}
} // namespace execution_nodes