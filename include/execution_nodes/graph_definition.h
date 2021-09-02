#pragma once

#include <execution_nodes/node_definition.h>
#include <execution_nodes/connection.h>

namespace execution_nodes {

struct GraphDefinition {
	std::string name;
	std::vector<NodeDefinition> nodes;
	std::vector<ConnectionDefinition> connections;
};

GraphDefinition loadGraphDefFromJson(const std::string& jsonFilePath);
GraphDefinition loadGraphDefFromJson(const nlohmann::json& j);

} // namespace execution_nodes