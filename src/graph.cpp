#include <execution_nodes/graph.h>
#include <execution_nodes/topological_sort.h>

namespace execution_nodes {

size_t getIndexOfElement(const std::string &element,
                         const std::vector<std::string> &list) {
  for (size_t i = 0; i < list.size(); i++) {
    if (list[i] == element) {
      return i;
    }
  }
  Log().ErrorThrow() << "Error when sorting nodes. Unable to find node '"
                     << element << "' in the list of connections";
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
        Log().Info() << "Created node '" << nodeDefinition.name << "' of type '"
                     << nodeDefinition.type << "'.";
      } else {
        Log().ErrorThrow() << "Unable to create node of type '"
                           << nodeDefinition.type
                           << "'. The creation function is NULL.";
      }
    } else {
      Log().ErrorThrow()
          << "Unable to create node of type '" << nodeDefinition.type
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
    } else if (nodeName == connection.src.nodeName) {
      srcExists = true;
    }
  }

  if (srcExists == false) {
    Log().ErrorThrow() << "Error when adding connection. The source node '"
                       << connection.src.nodeName << "' does not exist.";
  }

  if (dstExists == false) {
    Log().ErrorThrow() << "Error when adding connection. The destination node '"
                       << connection.dst.nodeName << "' does not exist.";
  }
}

void Graph::addConnection(ConnectionDefinition connection, bool reorderNodes) {

  checkIfConnectionIsValid(connection);

  if (connections_.find(connection) == connections_.end()) {

    connections_.insert(connection);
    connector_->connect(connection.src, connection.dst);
    Log().Debug() << "Added connection " << toString(connection);
  } else {
    Log().Warning() << "Attempted to add a connection that does already exist.";
  }
  if (reorderNodes) {
    sortNodes();
  }
}

void Graph::removeConnection(ConnectionDefinition connection,
                             bool reorderNodes) {

  auto iter = connections_.find(connection);

  if (iter == connections_.end()) {
    Log().Warning() << "Attempted to remove a connection that does not exist.";
  } else {
    connections_.erase(iter);
  }
  connector_->disconnect(connection.src, connection.dst);
  Log().Debug() << "Removed connection " << toString(connection);

  if (reorderNodes) {
    sortNodes();
  }
}

void Graph::execute() {
  for (const auto &node : nodes_) {
    Log().Debug() << "Executing node '" << node->getName() << "'...";
    node->execute();
  }
}

void Graph::sortNodes() {

  Log().Debug() << "Sorting nodes...";

  std::vector<ConnectionDefinition> connectionVector(connections_.begin(),
                                                     connections_.end());
  std::vector<std::string> sortedNodeNames =
      getNodeExecutionOrder(connectionVector);

  Log().Debug() << "Order before sorting:";
  for (const auto &node : nodes_) {
    Log().Debug() << node->getName();
  }

  std::sort(std::begin(nodes_), std::end(nodes_),
            [&sortedNodeNames](const NodePtr &a, const NodePtr &b) {
              size_t aIdx = getIndexOfElement(a->getName(), sortedNodeNames);
              size_t bIdx = getIndexOfElement(b->getName(), sortedNodeNames);
              return aIdx < bIdx;
            });

  Log().Debug() << "Order after sorting:";
  for (const auto &node : nodes_) {
    Log().Debug() << node->getName();
  }
}

} // namespace execution_nodes