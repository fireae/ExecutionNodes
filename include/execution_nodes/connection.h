#pragma once
#include <cstdint>
#include <string>

namespace execution_nodes {

typedef std::string PortId;
typedef std::string ConnectionName;
typedef std::string ConnectionId;

static const std::string no_connection{""};

struct Port {
  std::string nodeName;
  std::string portName;
  Port() = default;
  Port(const std::string &nodeName_, const std::string &portName_)
      : nodeName(nodeName_), portName(portName_) {}
};

PortId createPortId(const Port &port);

PortId createPortId(const std::string &nodeName, const std::string &portName);

ConnectionName createConnectionName(const PortId &out, const PortId &in);

struct ConnectionDefinition {
  Port src;
  Port dst;
  ConnectionDefinition() = default;
  ConnectionDefinition(const Port &srcPort, const Port &dstPort)
      : src(srcPort), dst(dstPort) {}

  bool operator<(const ConnectionDefinition &rhs) const;
};

} // namespace execution_nodes