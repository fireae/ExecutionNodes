#include <execution_nodes/graph.h>

namespace execution_nodes {

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
        Log().Info() << nodeDefinition.name;
      } else {
        //log error
      }
    } else {
     // log error
    }


  }

  for(const auto& connection : graphDefinition.connections) {
    connector_->connect(connection.src, connection.dst);
  }

  //Todo: Sort nodes

}

void Graph::execute() {
  for (const auto &node : nodes_) {
    node->execute();
  }
}
} // namespace execution_nodes