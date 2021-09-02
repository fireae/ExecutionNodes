#pragma once

#include <nlohmann/json.hpp>

namespace execution_nodes {


struct NodeDefinition {
  std::string name;
  std::string type;
  nlohmann::json settings;

  NodeDefinition();
  NodeDefinition(const std::string nodeName, const std::string& nodeType, nlohmann::json nodeSettings);
  NodeDefinition(const std::string nodeName, const std::string &nodeType);
};

void from_json(const nlohmann::json &j, NodeDefinition &d);


} // namespace execution_nodes