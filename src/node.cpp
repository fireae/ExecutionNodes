#pragma once
#include <execution_nodes/node.h>
#include <spdlog/spdlog.h>

namespace execution_nodes {

ConnectionId hash(const std::string &str) { return 0; }

Node::Node(const NodeDefinition &nodeDefinition,
           const std::shared_ptr<Connector> &connector)
    : name_(nodeDefinition.name), type_(nodeDefinition.type),
      settings_(nodeDefinition.settings), connector_(connector) {

  for (const auto &i : nodeDefinition.inputs) {
    inputs_[i.first] = hash(i.second);
    portTypeMap_[i.first] = PortType::INPUT;
  }

  for (const auto &o : nodeDefinition.outputs) {

    if (portTypeMap_.find(o.first) != portTypeMap_.end()) {

      spdlog::error("The port name {} is already defined as input port. Port "
                    "names must be unique per node.",
                    o.first);
      throw std::runtime_error("todo");
    } else {
      outputs_[o.first] = hash(o.second);
      portTypeMap_[o.first] = PortType::OUTPUT;
    }
  }
}

} // namespace execution_nodes