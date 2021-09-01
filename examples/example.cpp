#include <execution_nodes/execution_nodes.h>
#include <execution_nodes/node.h>
#include <iostream>
#include <random>

using namespace execution_nodes;

class RNG : public execution_nodes::Node {

public:
  RNG(const NodeDefinition &definition,
      const ConnectorPtr &connector)
      : execution_nodes::Node(definition, connector) {

    uint64_t seed = getSetting<uint64_t>("seed");
    randomEngine_ = std::mt19937_64(seed);
    randomEngine_.discard(10000000);
  }

  void execute() override {

    int number = distr_(randomEngine_);
    setOutput("number", number);
  }

private:
  std::mt19937_64 randomEngine_;
  std::uniform_int_distribution<int> distr_;
};

int main() {}