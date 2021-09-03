#pragma once

#include <execution_nodes/connection.h>
#include <list>
#include <string>
#include <vector>

namespace execution_nodes {

std::vector<std::string /*node name*/>
getNodeExecutionOrder(const std::vector<ConnectionDefinition> edges);

}