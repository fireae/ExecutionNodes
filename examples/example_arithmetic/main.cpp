#include <execution_nodes/execution_nodes.h>
#include <iostream>
#include <random>

using namespace execution_nodes;

class RandomNumberGenerator : public execution_nodes::Node {

public:
  RandomNumberGenerator(const NodeDefinition &definition,
                        const ConnectorPtr &connector)
      : execution_nodes::Node(definition, connector),
        distr_(std::uniform_int_distribution<int>(0, 100)) {

    uint64_t seed = getSetting<uint64_t>("seed");
    randomEngine_ = std::mt19937_64(seed);
    randomEngine_.discard(10000000);
  }

  void execute() override {

    int number = distr_(randomEngine_);
    setOutput("number", static_cast<float>(number));
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

    float a = 0, b = 0;
    getInput("a", a);
    getInput("b", b);

    float sum = a + b;
    std::cout << a << "+" << b << " = " << sum << std::endl;

    setOutput("sum", sum);
  }
};

class Squarer : public execution_nodes::Node {

public:
  Squarer(const NodeDefinition &definition, const ConnectorPtr &connector)
      : execution_nodes::Node(definition, connector) {}

  void execute() override {

    float x = 0;
    getInput("x", x);
    setOutput("x2", x * x);
  }
};

class Divider : public execution_nodes::Node {

public:
  Divider(const NodeDefinition &definition, const ConnectorPtr &connector)
      : execution_nodes::Node(definition, connector) {}

  void execute() override {

    float a = 0;
    float b = 0;
    getInput("a", a);
    getInput("b", b);
    setOutput("quotient", a / b);
  }
};

class NumberPrinter : public execution_nodes::Node {

public:
  NumberPrinter(const NodeDefinition &definition, const ConnectorPtr &connector)
      : execution_nodes::Node(definition, connector) {}

  void execute() override {
    float number = 0;
    getInput("number", number);
    std::cout << "The number is: " << number << std::endl;
  }
};

int main() {

  NodeRegistry registry = {
      REGISTER(Adder),
      REGISTER(Squarer),
      REGISTER(RandomNumberGenerator),
      REGISTER(Divider),
      REGISTER(NumberPrinter),
  };

  std::string filePath =
      "C:/Users/Sebastian/source/repos/ExecutionNodes/examples/arithmetic.json";

  GraphDefinition graphDef = loadGraphDefFromJsonFile(filePath);

  Graph graph(graphDef, registry);
  while (true) {
    // while (true) {
    graph.executeSerial();
    graph.executeParallel();
    //};
  }

  /*
  ConnectorPtr connector = std::make_shared<Connector>();

  NodeDefinition defRng1("rng1", "RandomNumberGenerator", {{"seed", 42}});
  NodeDefinition defRng2("rng2", "RandomNumberGenerator", {{"seed", 69}});
  NodeDefinition defAdder("adder", "Adder");
  NodeDefinition defPrinter("printer", "NumberPrinter");

  std::unique_ptr<Node> node = registry.at("Adder")(defRng1, connector);

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
  */
}