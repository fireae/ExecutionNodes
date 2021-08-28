#pragma once

#include <execution_nodes/connector.h>

#include <string>

#include <spdlog/spdlog.h>

#include <functional>

namespace execution_nodes {

static const std::hash<std::string> stringHasher;

ConnectionId hash(const std::string &str) {
  return static_cast<ConnectionId>(stringHasher(str));
}

PortId createPortId(const std::string &nodeName, const std::string &portName) {
	return nodeName + ":" + portName;
}


Connector::Connector(){

};

Connector::registerConnection(const std::string &nodeName,
                              const std::string &portName,
                              const std::string &connection) {
}

} // namespace execution_nodes