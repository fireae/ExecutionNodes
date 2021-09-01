#include <execution_nodes/logging.hpp>
#include <execution_nodes/node_definition.h>

namespace execution_nodes {

void from_json(const nlohmann::json &j, NodeDefinition &d) {

  try {

    j.at("name").get_to(d.name);
    j.at("type").get_to(d.type);

  } catch (const std::exception &ex) {
    /*
      Log::error("Unable to parse name or type of node definition from json: {}",
               ex.what());
    */
    throw;
  }

  try {

    // if (j.find("inputs") != j.end()) {
    //  for (const auto &item : j["inputs"].items()) {
    //    if (item.value().is_string()) {
    //      d.inputs[item.key()] = item.value();
    //    }
    //  }
    //}

    // if (j.find("outputs") != j.end()) {
    //  for (const auto &item : j["outputs"].items()) {
    //    if (item.value().is_string()) {
    //      d.outputs[item.key()] = item.value();
    //    }
    //  }
    //}

    d.settings = j.at("settings").get<nlohmann::json>();
  } catch (const std::exception &ex) {
    /*
      Log::error(
        "Unable to parse node definition for node {} of type {} from json: {}",
        d.name, d.type, ex.what());
    */
    throw;
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