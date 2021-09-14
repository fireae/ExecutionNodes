#include <execution_nodes/connection.h>
#include <execution_nodes/internal/helpers.h>
#include <execution_nodes/internal/logging.h>
#include <vector>

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

ConnectionDefinition::ConnectionDefinition(const std::string &srcId,
                                           const std::string &dstId) {

  auto srcTokens = split(srcId, ':');
  if (srcTokens.size() == 2) {
    src = Port(srcTokens[0], srcTokens[1]);
  } else {
    LOG_ERROR << "Invalid port id: " << srcId;
  }

  auto dstTokens = split(dstId, ':');
  if (dstTokens.size() == 2) {
    dst = Port(dstTokens[0], dstTokens[1]);
  } else {
    LOG_ERROR << "Invalid port id: " << dstId;
  }
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