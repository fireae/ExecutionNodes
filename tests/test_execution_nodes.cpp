#include <catch2/catch_test_macros.hpp>
#include <execution_nodes/execution_nodes.h>

using namespace execution_nodes;

static std::vector<std::string> executionTrack = {};

class TestSource : public Node {
public:
  TestSource(NodeDefinition d, ConnectorPtr c)
      : Node(d, c), seed(getSetting<int>("seed")) {}

  void execute() {
    executionTrack.emplace_back(getName());
    setOutput("out", seed);
  }

private:
  int seed;
};

class TestSink : public Node {
public:
  TestSink(NodeDefinition d, ConnectorPtr c)
      : Node(d, c), expected(getSetting<int>("expected")) {}

  void execute() {
    executionTrack.emplace_back(getName());
    int in;
    getInput("in", in);
    REQUIRE(in == expected);
  }

private:
  int expected;
};

class DummyNode : public Node {
public:
  DummyNode(NodeDefinition d, ConnectorPtr c) : Node(d, c) {}

  void execute() {
    executionTrack.emplace_back(getName());
    int in;
    getInput("in", in);
    int out = in;
    setOutput("out", out);
  }
};

static const NodeRegistry registry = {
    REGISTER(TestSource),
    REGISTER(DummyNode),
    REGISTER(TestSink),
};

std::shared_ptr<Graph> constructABC(int seed) {

  nlohmann::json sourceSettings;
  sourceSettings["seed"] = seed;

  nlohmann::json sinkSettings;
  sinkSettings["expected"] = seed;

  GraphDefinition gd;
  gd.name = "test";

  auto A = NodeDefinition("A", "TestSource", sourceSettings);
  gd.nodes.emplace_back("C", "TestSink", sinkSettings);
  gd.nodes.emplace_back("B", "DummyNode");
  gd.nodes.emplace_back(A);

  gd.connections.emplace_back(Port("A", "out"), Port("B", "in"));
  auto cd = ConnectionDefinition(Port("B", "out"), Port("C", "in"));
  gd.connections.emplace_back(cd);

  executionTrack = {};

  return std::make_shared<Graph>(Graph(gd, registry));
}

TEST_CASE("Construct Simple Graph", "Test") { auto graph = constructABC(42); }

TEST_CASE("Graph hasNode()", "Test") {
  auto graph = constructABC(42);
  REQUIRE(graph->hasNode("A") == true);
  REQUIRE(graph->hasNode("B") == true);
  REQUIRE(graph->hasNode("C") == true);
  REQUIRE(graph->hasNode("does not exist") == false);
  REQUIRE(executionTrack.size() == 0); // no node was executed
}

TEST_CASE("Execute Simple Graph", "Test") {
  auto graph = constructABC(42);
  graph->execute();
  REQUIRE(executionTrack.size() == 3); // exactly the nodes where executed
  REQUIRE(executionTrack[0] == "A");   // first node was A
  REQUIRE(executionTrack[1] == "B");   // then B
  REQUIRE(executionTrack[2] == "C");   // then C
}

TEST_CASE("Remove Connection from Graph", "Test") {
  auto graph = constructABC(42);
  graph->removeConnection(
      ConnectionDefinition(Port("B", "out"), Port("C", "in")));
  REQUIRE(graph->hasNode("C") == false);
  graph->execute();
  REQUIRE(executionTrack.size() == 2);
  REQUIRE(executionTrack[0] == "A");
  REQUIRE(executionTrack[1] == "B");
}

TEST_CASE("Add Node to Graph", "Test") {
  auto graph = constructABC(42);
  graph->removeConnection(
      ConnectionDefinition(Port("B", "out"), Port("C", "in")));
  REQUIRE(graph->hasNode("C") == false);
  nlohmann::json setting;
  setting["expected"] = 42;
  graph->addNode(NodeDefinition("C", "TestSink", setting),
                 {ConnectionDefinition("B:out", "C:in")});
  REQUIRE(graph->hasNode("C") == true);
  graph->execute();
  REQUIRE(executionTrack.size() == 3);
  REQUIRE(executionTrack[0] == "A");
  REQUIRE(executionTrack[1] == "B");
  REQUIRE(executionTrack[2] == "C");
}

TEST_CASE("Remove Nodes from Graph", "Test") {
  auto graph = constructABC(42);
  graph->removeNode("B");
  REQUIRE(graph->hasNode("A") == true);
  REQUIRE(graph->hasNode("B") == false);
  REQUIRE(graph->hasNode("C") == true);
  graph->removeNode("C");
  REQUIRE(graph->hasNode("A") == true);
  REQUIRE(graph->hasNode("B") == false);
  REQUIRE(graph->hasNode("C") == false);
  graph->execute();
  REQUIRE(executionTrack.size() == 1);
  REQUIRE(executionTrack[0] == "A");
}

TEST_CASE("Add Connection to Graph", "Test") {

  auto graph = constructABC(42);
  graph->removeNode("B");
  REQUIRE(graph->hasNode("A") == true);
  REQUIRE(graph->hasNode("B") == false);
  REQUIRE(graph->hasNode("C") == true);
  graph->addConnection(ConnectionDefinition("A:out", "C:in"));
  graph->execute();
  REQUIRE(executionTrack.size() == 2);
  REQUIRE(executionTrack[0] == "A");
  REQUIRE(executionTrack[1] == "C");

}
