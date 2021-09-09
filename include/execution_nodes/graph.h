#pragma once

#include <execution_nodes/graph_definition.h>
#include <execution_nodes/node.h>
#include <execution_nodes/node_registry.h>
#include <set>

namespace execution_nodes {

/**
 * @brief The graph holds all the nodes in the right order of execution and all
 * connections between the nodes.
 */
class Graph {

public:
  /**
   * @brief Construct a new Graph by a given @ref graph definition and a @ref
   * registry.
   *
   * @param graphDefinition The graph definition defines how to build the graph
   * and which nodes are included.
   * @param registry The registry used for instantiating the nodes.
   */
  explicit Graph(const GraphDefinition &graphDefinition,
                 const NodeRegistry &registry);

  /**
   * @brief Execute each node in the graph in the right order of execution.
   */
  void execute();

  /**
   * @brief Add a new connection between two nodes (ports).
   * Reflexive connections (connection from a port to itself) are forbidden.
   * @param connection The connection that shall be added.
   * @param reorderNodes If set to true the nodes will be re-ordered so the
   * order of execution will be updated according to the newly added connection.
   */
  void addConnection(ConnectionDefinition connection, bool reorderNodes = true);
  /**
   * @brief Remove an existing connection between two nodes (ports)
   *
   * @param connection The connection which shall be removed.
   * @param reorderNodes If set to true the nodes will be re-ordered so the
   * order of execution will be updated according to the removed connection.
   */
  void removeConnection(ConnectionDefinition connection,
                        bool reorderNodes = true);

private:
  // Name of this graph
  std::string name_;
  // The connector pointer is shared inside each node.
  std::shared_ptr<Connector> connector_;
  // The registry used for instantiating the nodes.
  NodeRegistry registry_;
  // A list containing all nodes in the graph. Nodes in this list are kept in
  // execution order.
  std::vector<std::unique_ptr<Node>> nodes_;
  // A list with all connections.
  std::set<ConnectionDefinition> connections_;
  /**
   * @brief Sorts the nodes
   */
  void sortNodes();
  /**
   * @brief Checks whether a connection is a valid connection. A connection is
   * valid if both source port and destination port exist.
   *
   * @param connection
   */
  void checkIfConnectionIsValid(const ConnectionDefinition &connection);
};

} // namespace execution_nodes