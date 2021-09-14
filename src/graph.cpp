#include <execution_nodes/graph.h>
#include <execution_nodes/internal/connector.h>
#include <execution_nodes/internal/helpers.h>
#include <execution_nodes/internal/logging.h>
#include <execution_nodes/internal/topological_sort.h>
#include <execution_nodes/thirdparty/thread-pool-2.0.0/thread_pool.hpp>

namespace execution_nodes {

static thread_pool threadPool;

struct Graph::HiddenTypeMembers {

  SortedNodes order;
};

Graph::Graph(const GraphDefinition &graphDefinition,
             const NodeRegistry &registry)
    : name_(graphDefinition.name),
      connector_(std::shared_ptr<Connector>(new Connector())),
      registry_(registry),
      hidden_(std::make_shared<HiddenTypeMembers>(HiddenTypeMembers())) {
  for (const auto &nodeDefinition : graphDefinition.nodes) {
    createAndAddNode(nodeDefinition);
  }

  for (const auto &connection : graphDefinition.connections) {
    addConnection(connection, false);
  }

  sortNodes();
}

void Graph::addNode(const NodeDefinition &node,
                    const std::vector<ConnectionDefinition> &connections) {
  if (connections.size() == 0) {
    THROW_ERROR << "No connections specified. You must specify at least one "
                   "connection and all connections specified must be from or "
                   "to this new node.";
  }

  // check if the connection actually belongs to the newly to be added node.
  for (const auto &c : connections) {
    bool nodeIsSrc = (c.src.nodeName == node.name);
    bool nodeIsDst = (c.dst.nodeName == node.name);
    if ((!nodeIsDst) && (!nodeIsSrc)) {
      THROW_ERROR << "The connection '" << helpers::toString(c)
                  << "' does neither start nor end at the node '" << node.name
                  << "' to be added.";
    }
  }

  // only after we made sure the connections are all good, the node can be
  // added.
  createAndAddNode(node);

  bool allConnectionsValid = true;

  for (const auto &c : connections) {
    allConnectionsValid &= checkIfConnectionIsValid(c, false);
  }

  if (!allConnectionsValid) {
    LOG_ERROR << "At least one connection is invalid. The node will be "
                 "removed again.";
    removeNode(node.name);
    return;
  }

  // now that the node is here it is time to add the connections.
  for (const auto &c : connections) {
    addConnection(c, false);
  }
  // finally, sort the nodes.
  sortNodes();
}

void Graph::removeNode(const std::string &nodeName) {
  if (!hasNode(nodeName)) {
    LOG_WARNING << "The node '" << nodeName << "' does not exist.";
    return;
  }

  disconnectNode(nodeName);

  auto it = nodes_.begin();
  while (it != nodes_.end()) {
    auto &node = *it;
    if (node->getName() == nodeName) {
      it = nodes_.erase(it);
      LOG_INFO << "Removed node '" << nodeName << "'.";
    } else {
      ++it;
    }
  }
}

bool Graph::hasNode(const std::string &nodeName) {
  for (const auto &node : nodes_) {
    if (node->getName() == nodeName) {
      return true;
    }
  }
  return false;
}

void Graph::createAndAddNode(const NodeDefinition &node) {
  if (hasNode(node.name)) {
    LOG_ERROR << "The node '" << node.name << "' of type '" << node.type
              << "' can not be created because there already is a node with "
                 "that name. Please make sure your names are unique.";
    return;
  }

  auto iter = registry_.find(node.type);

  if (iter != registry_.end()) {

    const auto &create = iter->second;
    if (create != nullptr) {
      nodes_.emplace_back(create(node, connector_));
      LOG_INFO << "Created node '" << node.name << "' of type '" << node.type
               << "'.";
    } else {
      THROW_ERROR << "Unable to create node of type '" << node.type
                  << "'. The creation function is NULL.";
    }
  } else {
    THROW_ERROR << "Unable to create node of type '" << node.type
                << "'. This node type can not be found in the registry.";
  }
}

void Graph::getOutputInternal(const Port &outputPort, std::any &anyObj) {
  auto portId = createPortId(outputPort);
  connector_->getObjectFromOutput(portId, anyObj);
}

void Graph::fakeOutputInternal(const Port &outputPort, const std::any &anyObj) {
  auto portId = createPortId(outputPort);
  connector_->setObject(portId, anyObj);
}

void Graph::reportError(const std::string &msg) { THROW_ERROR << msg; }

void Graph::disconnectNode(const std::string &nodeName) {
  auto iter = connections_.begin();
  LOG_INFO << "Removing all connections from node '" << nodeName << "'...";
  while (iter != connections_.end()) {
    if ((iter->dst.nodeName == nodeName) || (iter->src.nodeName == nodeName)) {
      removeConnection(*iter, false);
      iter = connections_.begin();
    } else {
      iter++;
    }
  }
}

bool Graph::checkIfConnectionIsValid(const ConnectionDefinition &connection,
                                     bool doThrow) {
  bool dstExists = false;
  bool srcExists = false;

  for (const auto &node : nodes_) {
    std::string nodeName = node->getName();

    if (nodeName == connection.dst.nodeName) {
      dstExists = true;
    }
    if (nodeName == connection.src.nodeName) {
      srcExists = true;
    }
  }

  if ((srcExists == false) && doThrow) {

    THROW_ERROR << "Error when adding connection. The source node '"
                << connection.src.nodeName << "' does not exist.";
  }

  if ((dstExists == false) && doThrow) {
    THROW_ERROR << "Error when adding connection. The destination node '"
                << connection.dst.nodeName << "' does not exist.";
  }
  return srcExists && dstExists;
}

void Graph::addConnection(ConnectionDefinition connection, bool reorderNodes) {
  checkIfConnectionIsValid(connection, true);

  if (connections_.find(connection) == connections_.end()) {

    connections_.insert(connection);
    connector_->connect(connection.src, connection.dst);
    LOG_DEBUG << "Added connection " << helpers::toString(connection);
  } else {
    LOG_WARNING << "Attempted to add a connection that does already exist.";
  }
  if (reorderNodes) {
    sortNodes();
  }
}

void Graph::removeConnection(ConnectionDefinition connection,
                             bool reorderNodes) {
  auto iter = connections_.find(connection);

  if (iter == connections_.end()) {
    LOG_WARNING << "Attempted to remove a connection that does not exist.";
  } else {
    connections_.erase(iter);
  }
  connector_->disconnect(connection.src, connection.dst);
  LOG_DEBUG << "Removed connection " << helpers::toString(connection);

  if (reorderNodes) {
    sortNodes();
  }
}

void Graph::execute(ExecutionMode mode) {
  if (mode == ExecutionMode::PARALLEL) {
    executeParallel();
  } else if (mode == ExecutionMode::SERIAL) {
    executeSerial();
  }
}

void Graph::executeSerial() {

  helpers::Timer timer;
  timer.start();

  connector_->clearObjects();

  for (const auto &node : nodes_) {
    std::string nodeName = node->getName();
    std::string nodeType = node->getType();
    LOG_DEBUG << "Executing node '" << nodeName << "'";
    try {
      LOG_DEBUG << "...";
      node->execute();
      LOG_DEBUG << "Done";
    } catch (const std::exception &ex) {
      THROW_ERROR << "Error when executing node '" << nodeName << "' of type '"
                  << nodeType << "'. Addition info: " << ex.what();
    }
  }

  LOG_DEBUG << "Execution took " << timer.stop() << "ms";
}

uint32_t Graph::getParallelThreadCount() {
  return static_cast<uint32_t>(threadPool.get_thread_count());
}

void Graph::setParallelThreadCount(uint32_t count) { threadPool.reset(count); }

void Graph::executeParallel() {

  helpers::Timer timer;
  timer.start();
  connector_->clearObjects();

  // finished: The set of all nodes which have already been executed
  std::set<std::string> finished;
  // running: The set of all nodes which are currently being executed
  std::set<std::string> running;
  // queued: The set of all nodes waiting for being executed
  std::set<std::string> queued;
  // This map contains for each node (name) a future object. The future object
  // is used for determining whether the node has finished execution.
  std::map<std::string, std::future<bool>> futuresMap;

  /*
  Following algorithm is used for executing the nodes in parallel:
  predecessorMap <- A map that contains a complete set of all predecessors for
  each node. So for each node N we know which other nodes P1, P2, ... Pn must be
  executed first in order to execute node N.


  1. Put all nodes into the set of queued nodes Q
  2. Let the set of all finished nodes F be an empty set {}

  3. While the set of queued nodes Q is not empty, do:

      4. Get all nodes N from the set of queued nodes Q where its predecessors
  PN are a subset of the set of finished nodes F

      5. Push all nodes N into a thread pool. Move them from the queued set Q to
         the set of running nodes R

      6. Now wait until at least one node is finished

      7. Move all finished nodes from the set of running nodes R to the set of
         finished nodes F

      8. Goto 3.

  9. Wait until all reamining running nodes are finished
  */

  // Step 1. Step 2. was already done implicitly when creating the set.
  for (auto it = nodes_.begin(); it != nodes_.end(); it++) {
    const Node *node = it->get();
    queued.insert(node->getName());
  }

  // Step 3.
  while (queued.size() != 0) {

    // Step 4.
    auto batch = helpers::getDependingNodes(finished, queued,
                                            hidden_->order.predecessorMap);

    std::string msg = "";

    // Step 5.
    for (const auto &nodeName : batch) {
      auto iter = nodeNameIndexMap_.find(nodeName);
      if (iter != nodeNameIndexMap_.end()) {
        // construct a fancy message
        if (msg == "") {
          msg = "Executing node(s) '" + nodeName + "'";
        } else {
          msg += (" | '" + nodeName + "'");
        }

        size_t index = nodeNameIndexMap_[nodeName];
        Node *node = nodes_[index].get();
        // remove from the set of queued nodes
        queued.erase(nodeName);
        // add to the set of running nodes
        running.insert(nodeName);
        // push the node execution into the thread pool and store the future.
        // the future will be used to check whether a node is finished.
        futuresMap[nodeName] = threadPool.submit([node] { node->execute(); });
      }
    }
    LOG_DEBUG << msg << "\0";
    LOG_DEBUG << "Waiting...";
    // Step 6. and Step 7.
    auto done = helpers::waitForAny(futuresMap, finished, running);

    LOG_DEBUG << "These node(s) are done: " << helpers::setToString(done);
  }

  auto stillRunning = threadPool.get_tasks_running();
  if (stillRunning > 0) {

    LOG_DEBUG << "Waiting for " << stillRunning << " remaining nodes...";
  }
  // Step 9.
  threadPool.wait_for_tasks();
  LOG_DEBUG << "All nodes are done.";

  LOG_DEBUG << "Parallel Execution took " << timer.stop() << "ms";
}

void Graph::sortNodes() {
  LOG_DEBUG << "Sorting nodes...";

  std::vector<ConnectionDefinition> connectionVector(connections_.begin(),
                                                     connections_.end());
  hidden_->order = getNodeExecutionOrder(connectionVector);

  nodeNameIndexMap_.clear();

  LOG_DEBUG << "Order before sorting:";

  auto it = nodes_.begin();
  while (it != nodes_.end()) {
    const auto &node = *it;
    const auto &nodeName = node->getName();

    size_t index = helpers::getIndexOfElement(
        node->getName(), hidden_->order.linearExecutionOrder, false);

    if (index > hidden_->order.linearExecutionOrder.size()) {
      // remove this node
      it = nodes_.erase(it);
      LOG_DEBUG << "Node '" << nodeName
                << "' has no connections and will be removed";

    } else {

      ++it;
      LOG_DEBUG << "Node '" << nodeName << "' will remain";
    }
  }

  std::sort(std::begin(nodes_), std::end(nodes_),
            [&](const NodePtr &a, const NodePtr &b) {
              size_t aIdx = helpers::getIndexOfElement(
                  a->getName(), hidden_->order.linearExecutionOrder, true);
              size_t bIdx = helpers::getIndexOfElement(
                  b->getName(), hidden_->order.linearExecutionOrder, true);
              return aIdx < bIdx;
            });

  LOG_DEBUG << "Order or execution after sorting:";

  std::string nodeNameList = "";
  size_t index = 0;
  for (const auto &node : nodes_) {
    if (nodeNameList != "") {
      nodeNameList += " -> ";
    }
    auto nodeName = node->getName();
    nodeNameList += nodeName;
    nodeNameIndexMap_[nodeName] = index;
    index++;
  }
  LOG_DEBUG << nodeNameList << '\0';
}

} // namespace execution_nodes