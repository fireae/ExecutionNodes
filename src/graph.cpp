#include <execution_nodes/graph.h>

namespace execution_nodes {

	Graph::Graph() {
	}
	
	void Graph::execute() {
		for(const auto& node : nodes_) {
			node->execute();
		}
	}
}