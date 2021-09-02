#pragma once
#include <cstdint>
#include <string>

namespace execution_nodes {

typedef size_t ConnectionId;

static constexpr ConnectionId no_connection {static_cast<ConnectionId>(0)};

struct Port {
  std::string nodeName;
  std::string portName;
  Port() = default;
  Port(const std::string &nodeName_, const std::string &portName_)
      : nodeName(nodeName_), portName(portName_) {}
};

struct ConnectionDefinition {
  Port src;
  Port dst;
  ConnectionDefinition() = default;
  ConnectionDefinition(const Port &srcPort, const Port &dstPort)
      : src(srcPort), dst(dstPort) {}
};

}