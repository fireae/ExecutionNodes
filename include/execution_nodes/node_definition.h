#pragma once

#include <map>
#include <nlohmann/json.hpp>
#include <string>

namespace execution_nodes {


struct NodeDefinition {
  std::string name;
  std::string type;
  //std::map<std::string /*name*/, std::string /*connection*/> inputs;
  //std::map<std::string /*name*/, std::string /*connection*/> outputs;
  nlohmann::json settings;

  NodeDefinition();
  NodeDefinition(const std::string nodeName, const std::string& nodeType, nlohmann::json nodeSettings);
  NodeDefinition(const std::string nodeName, const std::string &nodeType);
};

void from_json(const nlohmann::json &j, NodeDefinition &d);


} // namespace execution_nodes