#include <execution_nodes/graph.h>
#include <execution_nodes/topological_sort.h>

namespace execution_nodes {

size_t getIndexOfElement(const std::string &element,
                         const std::vector<std::string> &list, bool doThrow) {
  for (size_t i = 0; i < list.size(); i++) {
    if (list[i] == element) {
      return i;
    }
  }
  if (doThrow) {
    THROW_ERROR << "Error when sorting nodes. Unable to find node '" << element
                << "' in the list of connections";
  }
  return list.size() + 1;
}

Graph::Graph(const GraphDefinition &graphDefinition,
             const NodeRegistry &registry)
    : name_(graphDefinition.name),
      connector_(std::make_shared<Connector>(Connector())),
      registry_(registry) {

  for (const auto &nodeDefinition : graphDefinition.nodes) {

    auto iter = registry_.find(nodeDefinition.type);

    if (iter != registry_.end()) {

      const auto &create = iter->second;
      if (create != nullptr) {
        nodes_.emplace_back(create(nodeDefinition, connector_));
        LOG_INFO << "Created node '" << nodeDefinition.name << "' of type '"
                 << nodeDefinition.type << "'.";
      } else {
        THROW_ERROR << "Unable to create node of type '" << nodeDefinition.type
                    << "'. The creation function is NULL.";
      }
    } else {
      THROW_ERROR << "Unable to create node of type '" << nodeDefinition.type
                  << "'. This node type can not be found in the registry.";
    }
  }

  for (const auto &connection : graphDefinition.connections) {
    addConnection(connection, false);
  }

  sortNodes();
}

std::string toString(const ConnectionDefinition &cn) {
  return cn.src.nodeName + ":" + cn.src.portName + " -> " + cn.dst.nodeName +
         ":" + cn.dst.portName;
}

void Graph::checkIfConnectionIsValid(const ConnectionDefinition &connection) {

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

  if (srcExists == false) {
    THROW_ERROR << "Error when adding connection. The source node '"
                << connection.src.nodeName << "' does not exist.";
  }

  if (dstExists == false) {
    THROW_ERROR << "Error when adding connection. The destination node '"
                << connection.dst.nodeName << "' does not exist.";
  }
}

void Graph::addConnection(ConnectionDefinition connection, bool reorderNodes) {

  checkIfConnectionIsValid(connection);

  if (connections_.find(connection) == connections_.end()) {

    connections_.insert(connection);
    connector_->connect(connection.src, connection.dst);
    LOG_DEBUG << "Added connection " << toString(connection);
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
  LOG_DEBUG << "Removed connection " << toString(connection);

  if (reorderNodes) {
    sortNodes();
  }
}

void Graph::execute() {
  for (const auto &node : nodes_) {
    std::string nodeName = node->getName();
    std::string nodeType = node->getType();
    LOG_DEBUG << "Executing node '" << nodeName << "'...";
    try {

      node->execute();
    } catch (const std::exception &ex) {
      THROW_ERROR << "Error when executing node '" << nodeName << "' of type '"
                  << nodeType << "'. Addition info: " << ex.what();
    }
  }
}

void Graph::sortNodes() {

  LOG_DEBUG << "Sorting nodes...";

  std::vector<ConnectionDefinition> connectionVector(connections_.begin(),
                                                     connections_.end());
  std::vector<std::string> sortedNodeNames =
      getNodeExecutionOrder(connectionVector);

  LOG_DEBUG << "Order before sorting:";

  auto it = nodes_.begin();
  while (it != nodes_.end()) {
    const auto &node = *it;
    const auto &nodeName = node->getName();

    size_t index = getIndexOfElement(node->getName(), sortedNodeNames, false);

    if (index > sortedNodeNames.size()) {
      // remove this node
      it = nodes_.erase(it);
      LOG_DEBUG << "Node '" << nodeName << "' has no connections and will be removed";

    } else {
      ++it;
      LOG_DEBUG << "Node '" << nodeName << "' will remain";
    }
  }

  std::sort(std::begin(nodes_), std::end(nodes_),
            [&sortedNodeNames](const NodePtr &a, const NodePtr &b) {
              size_t aIdx = getIndexOfElement(a->getName(), sortedNodeNames, true);
              size_t bIdx = getIndexOfElement(b->getName(), sortedNodeNames, true);
              return aIdx < bIdx;
            });

  LOG_DEBUG << "Order or execution after sorting:";

  std::string nodeNameList = "";

  for (const auto &node : nodes_) {
      if(nodeNameList != "") {
        nodeNameList += " -> ";
      }
    nodeNameList += node->getName();
  }
  LOG_DEBUG << nodeNameList;
}

} // namespace execution_nodes