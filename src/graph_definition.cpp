#pragma once

#include <execution_nodes/graph_definition.h>
#include <execution_nodes/logging.hpp>

#include <execution_nodes/connection.h>
#include <fstream>

namespace execution_nodes {

std::vector<std::string> split(const std::string &str, char delimiter) {
  std::string line;
  std::vector<std::string> tokens;
  std::stringstream ss(str);
  while (std::getline(ss, line, delimiter)) {
    tokens.push_back(line);
  }
  return tokens;
}

Port portIdToPort(const std::string &str) {
  Port retval;
  auto tokens = split(str, ':');
  if (tokens.size() != 2) {
    Log().ErrorThrow() << "The string '" << str
                       << "' is no valid port definition. Please use for "
                          "example: 'nodeName:portName'";
  }
  retval.nodeName = tokens[0];
  retval.portName = tokens[1];
  return retval;
}

void from_json(const nlohmann::json &j, GraphDefinition &d) {

  try {

    j.at("name").get_to(d.name);

  } catch (const std::exception &ex) {
    Log().ErrorThrow() << "Unable to parse name of graph definition from json: "
                       << ex.what();
  }

  if (j.find("nodes") == j.end()) {
    Log().ErrorThrow() << "Unable to find nodes list in graph definition";
  }

  for (const auto &item : j.at("nodes").items()) {
    std::string nodeName = item.key();
    NodeDefinition nodeDefinition;
    nodeDefinition.name = nodeName;
    from_json(item.value(), nodeDefinition);
    d.nodes.push_back(nodeDefinition);
  }

  if (j.find("connections") == j.end()) {
    Log().ErrorThrow() << "Unable to find connections list in graph definition";
  }

  std::vector<std::pair<std::string /*src*/, std::string /*dst*/>>
      connectionsDef;

  j.at("connections").get_to(connectionsDef);

  for (const auto &pair : connectionsDef) {

    const auto &srcId = pair.first;
    const auto &dstId = pair.second;

    ConnectionDefinition connection;
    connection.src = portIdToPort(srcId);
    connection.dst = portIdToPort(dstId);

    d.connections.emplace_back(connection);
  }
}

std::string readFileToString(const std::string &filePath) {

  std::ifstream file;

  try {
    file.open(filePath);
  } catch (const std::exception &ex) {
    Log().ErrorThrow() << "Error when opening file from path '" << filePath
                       << "': " << ex.what();
  }

  if (!file.is_open()) {
    Log().ErrorThrow() << "Error when opening file from path '" << filePath
                       << "'";
  }
  std::string retval;

  try {
    std::stringstream ss;
    ss << file.rdbuf();
    retval = ss.str();
  } catch (const std::exception &ex) {
    Log().ErrorThrow() << "Error when reading file from path '" << filePath
                       << "': " << ex.what();
  }

  return retval;
}

GraphDefinition loadGraphDefFromJson(const std::string &jsonFilePath) {

  GraphDefinition retval;
  std::string content = readFileToString(jsonFilePath);
  nlohmann::json j;
  try {
    j = nlohmann::json::parse(content);
    retval = loadGraphDefFromJson(j);
  } catch (const std::exception &ex) {
    Log().ErrorThrow() << "Error when parsing content from file '"
                       << jsonFilePath << "'to json: " << ex.what();
  }
  return retval;
}
GraphDefinition loadGraphDefFromJson(const nlohmann::json &j) {

  GraphDefinition retval;
  from_json(j, retval);
  return retval;
}

} // namespace execution_nodes