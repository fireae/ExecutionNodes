#pragma once

#include <map>
#include <nlohmann/json.hpp>
#include <string>

namespace execution_nodes {

enum class PortType : uint8_t {
  INPUT,
  OUTPUT,
};

struct Port {
  PortType type;
  std::string connection;
};

struct NodeDefinition {
  bool enabled;
  std::string name;
  std::string type;
  std::map<std::string /*name*/, std::string /*connection*/> inputs;
  std::map<std::string /*name*/, std::string /*connection*/> outputs;
  nlohmann::json settings;
};

void from_json(const nlohmann::json &j, NodeDefinition &d);


} // namespace execution_nodes