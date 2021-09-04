#include <execution_nodes/logging.hpp>
#include <execution_nodes/node_definition.h>

namespace execution_nodes {

void from_json(const nlohmann::json &j, NodeDefinition &d) {

  try {

    j.at("type").get_to(d.type);

  } catch (const std::exception &ex) {
    THROW_ERROR
        << "Unable to parse name or type of node definition from json: "
        << ex.what();
  }

  if (j.find("settings") != j.end()) {
    d.settings = j.at("settings").get<nlohmann::json>();
  } else {
    d.settings = nullptr;
  }
}

NodeDefinition::NodeDefinition() {}
NodeDefinition::NodeDefinition(const std::string nodeName,
                               const std::string &nodeType,
                               nlohmann::json nodeSettings)
    : name(nodeName), type(nodeType), settings(nodeSettings) {}
NodeDefinition::NodeDefinition(const std::string nodeName,
                               const std::string &nodeType)
    : name(nodeName), type(nodeType)

{}

} // namespace execution_nodes