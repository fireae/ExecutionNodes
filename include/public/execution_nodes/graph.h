#pragma once

#include <execution_nodes/graph_definition.h>
#include <execution_nodes/node.h>
#include <execution_nodes/node_registry.h>
#include <set>

namespace execution_nodes {

enum class ExecutionMode : uint8_t {
  SERIAL,
  PARALLEL,
};

/**
 * @brief The graph holds all the nodes in the right order of execution and all
 * connections between the nodes.
 */
class Graph {

public:
  /**
   * @brief Construct a new Graph by a given @ref GraphDefinition definition and
   * a @ref NodeRegistry.
   *
   * @param graphDefinition The graph definition defines how to build the graph
   * and which nodes are included.
   * @param registry The registry used for instantiating the nodes.
   */
  explicit Graph(const GraphDefinition &graphDefinition,
                 const NodeRegistry &registry);

  /**
   * @brief Add a new node to the graph along with connections from or to this
   * node. After a new node was added, the nodes will be sorted.
   *
   * @param node The node to be added. Make sure the name of the node is unique
   * for the graph.
   * @param connections The connection to be added with this node. Please make
   * sure that all connection lead from or to this node.
   */
  void addNode(const NodeDefinition &node,
               const std::vector<ConnectionDefinition> &connections);

  /**
   * @brief Remove the node by the given node name. This will also remove all
   * connection to or from this node. After a node was removed, the nodes will
   * be sorted.
   *
   * @param nodeName The name of the node to be removed from the graph.
   */
  void removeNode(const std::string &nodeName);

  /**
   * @brief Check whether a node with the specified node name does exist in the
   * graph.
   *
   * @param nodeName The name of the node whos existence will be returned.
   * @return true If a node with this name does exist.
   * @return false If no node with this name does exist.
   */
  bool hasNode(const std::string &nodeName);

  /**
   * @brief Execute each node in the graph in the right order of execution.
   */
  void execute(ExecutionMode mode);

  void executeSerial();

  void executeParallel();

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

  /**
   * @brief Get object from an output port of a node within the graph. This is a
   * way to extract object from the graph that would normally only be accessible
   * from within a connected node.
   *
   * @tparam T The typename of the object.
   * @param outputPort The output port from which to get the object.
   * @param object The object that is returned.
   */
  template <class T> void getOutput(const Port &outputPort, T &object) {

    try {
      std::any anyObj;
      getOutputInternal(outputPort, anyObj);
      object = std::any_cast<T>(anyObj);
    } catch (const std::exception &ex) {
      std::string msg = "Error when getting object from port '" +
                        createPortId(outputPort) +
                        "'. Additional information: " + std::string(ex.what());
      reportError(msg);
    }
  }

  /**
   * @brief Fakes an output from a node. By calling this function you can inject
   * an object to all nodes which are connected to that output port. Say you
   * have the connection A -> B. By using this method you can put put objects to
   * node B without executing node A. However, If node A gets exeucted after you
   * called this method the output set will be overwritten by whatever A
   * produced.
   *
   * @tparam T The typename of the object to set. Can be any object.
   * @param outputPort The output port whose output should be faked.
   * @param object The object to be put.
   */
  template <class T> void fakeOutput(const Port &outputPort, const T &object) {
    fakeOutputInternal(outputPort, object);
  }

private:
  Graph() = default;
  /**
   * @brief Create a And Add Node object.
   *
   * @param node The node to be created and added.
   */
  void createAndAddNode(const NodeDefinition &node);

  void getOutputInternal(const Port &outputPort, std::any &anyObj);

  void fakeOutputInternal(const Port &outputPort, const std::any &anyObj);

  void reportError(const std::string &msg);

  /**
   * @brief Removes all connection from and to the node.
   *
   * @param nodeName The node name.
   */
  void disconnectNode(const std::string &nodeName);

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

  SortedNodes order_;
  std::map<std::string /*node name*/, size_t> nodeNameIndexMap_;

  /**
   * @brief Sorts the nodes
   */
  void sortNodes();
  /**
   * @brief Checks whether a connection is a valid connection. A connection is
   * valid if both source port and destination port exist.
   *
   * @param connection The connection which shall be checked.
   * @param doThrow Whether the method throws when the connection is invalid.
   * @return true if the connection is valid.
   * @return false otherwise.
   */
  bool checkIfConnectionIsValid(const ConnectionDefinition &connection,
                                bool doThrow);
};

} // namespace execution_nodes