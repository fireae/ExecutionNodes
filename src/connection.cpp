
#include <execution_nodes/connection.h>

namespace execution_nodes {

static const std::hash<std::string> stringHasher;

ConnectionId hash(const std::string &str) {
  return static_cast<ConnectionId>(stringHasher(str));
}

PortId createPortId(const Port &port) {
  return port.nodeName + ":" + port.portName;
}

PortId createPortId(const std::string &nodeName, const std::string &portName) {
  return nodeName + ":" + portName;
}

ConnectionName createConnectionName(const PortId &out, const PortId &in) {
  return out + "->" + in;
}

bool ConnectionDefinition::operator<(const ConnectionDefinition &rhs) const {

  auto rSrc = createPortId(rhs.src);
  auto rDst = createPortId(rhs.dst);
  auto rCnn = createConnectionName(rSrc, rDst);
  auto rId = hash(rCnn);

  auto lSrc = createPortId(src);
  auto lDst = createPortId(dst);
  auto lCnn = createConnectionName(lSrc, lDst);
  auto lId = hash(lCnn);

  return lId < rId;
}

} // namespace execution_nodes