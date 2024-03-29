#include <execution_nodes/execution_nodes.h>
#include <execution_nodes/node.h>
#include <execution_nodes/node_registry.h>
#include <iostream>
#include <random>

using namespace execution_nodes;

class RandomBool : public execution_nodes::Node {

public:
  RandomBool(const NodeDefinition &definition, const ConnectorPtr &connector)
      : execution_nodes::Node(definition, connector),
        distr_(std::uniform_int_distribution<int>(0, 100)) {

    uint64_t seed = getSetting<uint64_t>("seed");
    randomEngine_ = std::mt19937_64(seed);
  }

  void execute() override {

    int number = distr_(randomEngine_);
    bool o = (number % 2) == 0;
    setOutput("o", o);
  }

private:
  std::mt19937_64 randomEngine_;
  std::uniform_int_distribution<int> distr_;
};

class AndGate2 : public execution_nodes::Node {

public:
  AndGate2(const NodeDefinition &definition, const ConnectorPtr &connector)
      : execution_nodes::Node(definition, connector) {}

  void execute() override {

    bool i0, i1;
    getInput("i0", i0);
    getInput("i1", i1);

    bool o = i0 && i1;
    setOutput("o", o);
  }
};

class OrGate2 : public execution_nodes::Node {

public:
  OrGate2(const NodeDefinition &definition, const ConnectorPtr &connector)
      : execution_nodes::Node(definition, connector) {}

  void execute() override {

    bool i0, i1;
    getInput("i0", i0);
    getInput("i1", i1);

    bool o = i0 || i1;
    setOutput("o", o);
  }
};

class NotGate : public execution_nodes::Node {

public:
  NotGate(const NodeDefinition &definition, const ConnectorPtr &connector)
      : execution_nodes::Node(definition, connector) {}

  void execute() override {

    bool i;
    getInput("i", i);
    bool o = !i;
    setOutput("o", o);
  }
};

class AndGate : public execution_nodes::Node {

public:
  AndGate(const NodeDefinition &definition, const ConnectorPtr &connector)
      : execution_nodes::Node(definition, connector) {}

  void execute() override {

    auto allInputPortNames = getInputPortNames();

    bool o = true;

    for (const auto &portName : allInputPortNames) {
      bool in;
      getInput(portName, in);
      o &= in;
    }
    setOutput("o", o);
  }
};

class OrGate : public execution_nodes::Node {

public:
  OrGate(const NodeDefinition &definition, const ConnectorPtr &connector)
      : execution_nodes::Node(definition, connector) {}

  void execute() override {

    auto allInputPortNames = getInputPortNames();

    bool o = false;

    for (const auto &portName : allInputPortNames) {
      bool in;
      getInput(portName, in);
      o |= in;
    }
    setOutput("o", o);
  }
};

class NamedBoolPrinter : public execution_nodes::Node {

public:
  NamedBoolPrinter(const NodeDefinition &definition,
                   const ConnectorPtr &connector)
      : execution_nodes::Node(definition, connector),
        valueName(getSetting<std::string>("valueName")) {}

  void execute() override {
    bool i;
    getInput("i", i);
    std::cout << "The  value of " << valueName << " is: " << (i ? "True" : "False");
  }

private:
  std::string valueName;
};

static const NodeRegistry registry = {
    REGISTER(RandomBool),  //
    REGISTER(AndGate2),    //
    REGISTER(OrGate2),     //
    REGISTER(NotGate),     //
    REGISTER(NamedBoolPrinter), //
};

int main() {

  std::string filePath = "./concept_4.json";

  GraphDefinition graphDef = loadGraphDefFromJsonFile(filePath);

  Graph graph(graphDef, registry);
  graph.executeSerial();

}