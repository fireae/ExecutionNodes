#include <execution_nodes/execution_nodes.h>
#include <execution_nodes/node.h>
#include <iostream>
#include <random>

using namespace execution_nodes;

class RandomNumberGenerator : public execution_nodes::Node {

public:
  RandomNumberGenerator(const NodeDefinition &definition,
                        const ConnectorPtr &connector)
      : execution_nodes::Node(definition, connector),
        distr_(std::uniform_int_distribution<int>(0, 100))
  {

    uint64_t seed = getSetting<uint64_t>("seed");
    randomEngine_ = std::mt19937_64(seed);
    randomEngine_.discard(10000000);
  }

  void execute() override {

    int number = distr_(randomEngine_);
    setOutput("number", number);
    std::cout << "rng says: " << number << std::endl;
  }

private:
  std::mt19937_64 randomEngine_;
  std::uniform_int_distribution<int> distr_;
};

class Adder : public execution_nodes::Node {

public:
  Adder(const NodeDefinition &definition, const ConnectorPtr &connector)
      : execution_nodes::Node(definition, connector) {}

  void execute() override {

    int a = 0, b = 0;
    getInput("a", a);
    getInput("b", b);

    std::cout << "a: " << a << std::endl;
    std::cout << "b: " << b << std::endl;

    int sum = a + b;

    setOutput("sum", sum);
    std::cout << "sum: " << sum << std::endl;
  }
};

class NumberPrinter : public execution_nodes::Node {

public:
  NumberPrinter(const NodeDefinition &definition, const ConnectorPtr &connector)
      : execution_nodes::Node(definition, connector) {}

  void execute() override {
    int number = 0;
    getInput("number", number);
    std::cout << "The number is: " << number << std::endl;
  }
};

int main() {

  ConnectorPtr connector = std::make_shared<Connector>();

  NodeDefinition defRng1("rng1", "RandomNumberGenerator", {{"seed", 42}});
  NodeDefinition defRng2("rng2", "RandomNumberGenerator", {{"seed", 69}});
  NodeDefinition defAdder("adder", "Adder");
  NodeDefinition defPrinter("printer", "NumberPrinter");

  RandomNumberGenerator rng1(defRng1, connector);
  RandomNumberGenerator rng2(defRng2, connector);
  Adder adder(defAdder, connector);
  NumberPrinter printer(defPrinter, connector);

  connector->connect(Port("rng1", "number"), Port("adder", "a"));
  connector->connect(Port("rng2", "number"), Port("adder", "b"));
  connector->connect(Port("adder", "sum"), Port("printer", "number"));

  rng1.execute();
  rng2.execute();
  adder.execute();
  printer.execute();
}