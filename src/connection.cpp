
#include <execution_nodes/connection.h>

namespace execution_nodes {


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

  auto lSrc = createPortId(src);
  auto lDst = createPortId(dst);
  auto lCnn = createConnectionName(lSrc, lDst);

  return lCnn < rCnn;
}

} // namespace execution_nodes