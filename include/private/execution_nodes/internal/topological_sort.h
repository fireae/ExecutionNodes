#pragma once

#include <execution_nodes/connection.h>
#include <list>
#include <string>
#include <vector>

namespace execution_nodes {

/**
 * @brief Applies toplogical sorting of the nodes using depth-first search
 * according to their linear ordering such that for every connection
 * nodeA:portOut->nodeB:portIn from node A to node B, A comes before B in the
 * ordering. This ordering will be the order of execution of the nodes in the
 * graph. Nodes which are not connected to any other node (isolated) will be
 * removed. This function will only succeed if the graph is acyclic.
 *
 * @param edges The edges (connections) between the nodes
 * @return std::vector<std::string> The names of the nodes in execution order.
 */
std::vector<std::string /*node name*/>
getNodeExecutionOrder(const std::vector<ConnectionDefinition> edges);

} // namespace execution_nodes