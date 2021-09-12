#include <catch2/catch_test_macros.hpp>
#include <execution_nodes/execution_nodes.h>
#include <fstream>

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

class TestNode : public Node {
public:
  TestNode(NodeDefinition d, ConnectorPtr c) : Node(d, c) {}

  void execute() {
    executionTrack.emplace_back(getName());
    REQUIRE(hasInput("in") == true);
    REQUIRE(hasInput("zzz") == false);
    int zzz;
    bool exceptionOccured = false;
    try {
      exceptionOccured = false;
      getInput("zzz", zzz);
    } catch (const std::exception &e) {
      (void)e;
      exceptionOccured = true;
    }
    REQUIRE(exceptionOccured == true);

    auto inNames = getInputPortNames();

    REQUIRE(inNames.size() == 1);
    REQUIRE(*inNames.begin() == "in");

    auto outNames = getOutputPortNames();
    REQUIRE(outNames.size() == 1);
    REQUIRE(*outNames.begin() == "out");

    REQUIRE(getType() == "TestNode");

    try {
      exceptionOccured = false;
      setOutput("in", 5);
    } catch (const std::exception &e) {
      (void)e;
      exceptionOccured = true;
    }
    REQUIRE(exceptionOccured == true);

    try {
      exceptionOccured = false;
      setOutput("foo", 5);
    } catch (const std::exception &e) {
      (void)e;
      exceptionOccured = true;
    }
    REQUIRE(exceptionOccured == false);

    try {
      exceptionOccured = false;
      setOutput("out", 5);
    } catch (const std::exception &e) {
      (void)e;
      exceptionOccured = true;
    }
    REQUIRE(exceptionOccured == false);
  }
};

static const NodeRegistry registry = {
    REGISTER(TestSource),
    REGISTER(DummyNode),
    REGISTER(TestSink),
    REGISTER(TestNode),
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

std::shared_ptr<Graph> constructAB(int seed) {

  nlohmann::json sourceSettings;
  sourceSettings["seed"] = seed;

  GraphDefinition gd;
  gd.name = "test";

  auto A = NodeDefinition("A", "TestSource", sourceSettings);
  gd.nodes.emplace_back("B", "DummyNode");
  gd.nodes.emplace_back(A);

  gd.connections.emplace_back(Port("A", "out"), Port("B", "in"));

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
  graph->execute(ExecutionMode::SERIAL);
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
  graph->execute(ExecutionMode::SERIAL);
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
  graph->execute(ExecutionMode::SERIAL);
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
  graph->execute(ExecutionMode::SERIAL);
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
  graph->execute(ExecutionMode::SERIAL);
  REQUIRE(executionTrack.size() == 2);
  REQUIRE(executionTrack[0] == "A");
  REQUIRE(executionTrack[1] == "C");
}

TEST_CASE("Get Output from Node in Graph", "Test") {

  auto graph = constructABC(42);
  graph->execute(ExecutionMode::SERIAL);
  int bout;
  graph->getOutput(Port("B", "out"), bout);
  REQUIRE(bout == 42);
}

TEST_CASE("Fake Output of Node in Graph", "Test") {

  auto graph = constructABC(42);
  graph->fakeOutput(Port("A", "out"), 69);
  int aout;
  graph->getOutput(Port("A", "out"), aout);
  REQUIRE(aout == 69);
  graph->execute(ExecutionMode::SERIAL);
  graph->getOutput(Port("A", "out"), aout);
  REQUIRE(aout == 42);
}

TEST_CASE("Graph from JSON", "Test") {

  executionTrack = {};
  nlohmann::json j;
  // json string contains ABC graph definition just like in the other test
  // cases.
  j = nlohmann::json::parse(
      "{\"name\":\"Test ABC "
      "Graph\",\"nodes\":{\"A\":{\"type\":\"TestSource\",\"settings\":{"
      "\"seed\":42}},\"B\":{\"type\":\"DummyNode\"},"
      "\"C\":{\"type\":\"TestSink\",\"settings\":{\"expected\":42}}},"
      "\"connections\":[[\"A:out\",\"B:in\"],[\"B:out\",\"C:in\"]]}");

  auto gd = loadGraphDefFromJson(j);
  REQUIRE(gd.name == "Test ABC Graph");
  REQUIRE(gd.nodes.size() == 3);
  REQUIRE(gd.nodes[0].name == "A");
  REQUIRE(gd.nodes[0].type == "TestSource");
  REQUIRE(gd.nodes[1].name == "B");
  REQUIRE(gd.nodes[1].type == "DummyNode");
  REQUIRE(gd.nodes[2].name == "C");
  REQUIRE(gd.nodes[2].type == "TestSink");
  REQUIRE(gd.connections.size() == 2);

  auto graph = Graph(gd, registry);
  REQUIRE(graph.hasNode("A") == true);
  REQUIRE(graph.hasNode("B") == true);
  REQUIRE(graph.hasNode("C") == true);
  graph.execute(ExecutionMode::SERIAL);
  REQUIRE(executionTrack.size() == 3);
  REQUIRE(executionTrack[0] == "A");
  REQUIRE(executionTrack[1] == "B");
  REQUIRE(executionTrack[2] == "C");
}

TEST_CASE("Graph from JSON File", "Test") {

  executionTrack = {};

  std::string content =
      "{\"name\":\"Test ABC "
      "Graph\",\"nodes\":{\"A\":{\"type\":\"TestSource\",\"settings\":{"
      "\"seed\":42}},\"B\":{\"type\":\"DummyNode\"},"
      "\"C\":{\"type\":\"TestSink\",\"settings\":{\"expected\":42}}},"
      "\"connections\":[[\"A:out\",\"B:in\"],[\"B:out\",\"C:in\"]]}";

  std::ofstream file("./temp.json");
  file << content;
  file.close();
  auto gd = loadGraphDefFromJsonFile("./temp.json");
  std::remove("./temp.json");
  REQUIRE(gd.name == "Test ABC Graph");
  REQUIRE(gd.nodes.size() == 3);
  REQUIRE(gd.nodes[0].name == "A");
  REQUIRE(gd.nodes[0].type == "TestSource");
  REQUIRE(gd.nodes[1].name == "B");
  REQUIRE(gd.nodes[1].type == "DummyNode");
  REQUIRE(gd.nodes[2].name == "C");
  REQUIRE(gd.nodes[2].type == "TestSink");
  REQUIRE(gd.connections.size() == 2);

  auto graph = Graph(gd, registry);
  REQUIRE(graph.hasNode("A") == true);
  REQUIRE(graph.hasNode("B") == true);
  REQUIRE(graph.hasNode("C") == true);
  graph.execute(ExecutionMode::SERIAL);
  REQUIRE(executionTrack.size() == 3);
  REQUIRE(executionTrack[0] == "A");
  REQUIRE(executionTrack[1] == "B");
  REQUIRE(executionTrack[2] == "C");
}

TEST_CASE("Node Checks", "Test") {

  executionTrack = {};
  nlohmann::json j;
  // json string contains ABC graph definition just like in the other test
  // cases.
  j = nlohmann::json::parse(
      "{\"name\":\"Test ATC "
      "Graph\",\"nodes\":{\"A\":{\"type\":\"TestSource\",\"settings\":{"
      "\"seed\":42}},\"T\":{\"type\":\"TestNode\",\"settings\":null},"
      "\"C\":{\"type\":\"TestSink\",\"settings\":{\"expected\":5}}},"
      "\"connections\":[[\"A:out\",\"T:in\"],[\"T:out\",\"C:in\"]]}");

  auto gd = loadGraphDefFromJson(j);

  auto graph = Graph(gd, registry);
  REQUIRE(graph.hasNode("A") == true);
  REQUIRE(graph.hasNode("T") == true);
  REQUIRE(graph.hasNode("C") == true);
  graph.execute(ExecutionMode::PARALLEL);

  REQUIRE(executionTrack.size() == 3);
  REQUIRE(executionTrack[0] == "A");
  REQUIRE(executionTrack[1] == "T");
  REQUIRE(executionTrack[2] == "C");
}

TEST_CASE("Produce Unused Output", "Test") {

  executionTrack = {};

  auto graph = constructAB(42);

  graph->execute();

  REQUIRE(executionTrack.size() == 2);
  REQUIRE(executionTrack[0] == "A");
  REQUIRE(executionTrack[1] == "B");
    
}