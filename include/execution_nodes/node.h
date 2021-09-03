#pragma once

#include <execution_nodes/connector.h>
#include <execution_nodes/logging.hpp>
#include <execution_nodes/node_definition.h>
#include <memory>
#include <vector>

namespace execution_nodes {

class Node {
public:
  Node(const NodeDefinition &nodeDefinition, const ConnectorPtr &connector);
  virtual void execute() = 0;

  std::string getName();
  std::string getType();

protected:
  template <class T> void setOutput(const std::string &portName, const T &obj) {

    connector_->setObject(createPortId(name_, portName), obj);
  }
  bool hasInput(const std::string &portName);
  template <class T> void getInput(const std::string &portName, T &obj) {

    try {
      std::any anyObj;
      connector_->getObject(createPortId(name_, portName), anyObj);
      obj = std::any_cast<T>(anyObj);
    } catch (const std::bad_any_cast &bac) {

      Log().ErrorThrow() << "Error in node '" << name_ << "' of type '" << type_
                         << "' when getting object from input on port '"
                         << portName
                         << "'. This error might come from a wrong typename in "
                            "the template argument. Additional information: "
                         << bac.what();

    } catch (const std::exception &ex) {

      Log().ErrorThrow() << "Error in node '" << name_ << "' of type '" << type_
                         << "' when getting object from input on port '"
                         << portName
                         << "'. Additional information: " << ex.what();
    }
  }

  template <class T> T getSetting(const std::string &key) {
    return settings_[key].get<T>();
  }

private:
  std::string name_;
  std::string type_;
  nlohmann::json settings_;
  std::shared_ptr<Connector> connector_;
};

typedef std::unique_ptr<Node> NodePtr;

template <class DerivedNodeType>
NodePtr createNode(const NodeDefinition &nodeDefinition,
                                 const std::shared_ptr<Connector> &connector) {
  return std::make_unique<DerivedNodeType>(
      DerivedNodeType(nodeDefinition, connector));
}



} // namespace execution_nodes