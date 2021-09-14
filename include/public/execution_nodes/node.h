#pragma once

#include <any>
#include <execution_nodes/connection.h>
#include <execution_nodes/node_definition.h>
#include <memory>
#include <set>
#include <vector>

namespace execution_nodes {

class Connector;

typedef std::shared_ptr<Connector> ConnectorPtr;

/**
 * @brief Base class for each node. Inherit from this node in order to implement
 * your own node.
 *
 */
class Node {
public:
  /**
   * @brief Construct a new Node object using the @ref NodeDefinition and shared
   * pointer to a connector.
   *
   * @param nodeDefinition
   * @param connector
   */
  Node(const NodeDefinition &nodeDefinition,
       const std::shared_ptr<Connector> &connector);
  /**
   * @brief The execute function will be called by the graph. Each input and
   * output to and from the node shall be handeled in this function. The execute
   * function defined the behaviour of the node.
   *
   */
  virtual void execute() = 0;

  /**
   * @brief Get the name of this node. Node names shall be unique withing a
   * graph.
   * @return std::string The name of the node.
   */
  std::string getName() const;
  /**
   * @brief Get the type of the node. The type is equal to the name of the
   * derived node class. Using the macro @ref will ensure this behaviour.
   *
   * @return std::string The type of the node.
   */
  std::string getType() const;

protected:
  /**
   * @brief Set an object as output of the node to a specific output port. A
   * node can set as many outputs as it wants in any order. An object that was
   * set as output will be put to every connected node that followes in the
   * graph.
   *
   * @tparam T The type of the object that is set as output.
   * @param portName The name of the port.
   * @param obj The object that is set as output.
   * @throw runtime_error if the port name is undefined or if it does not
   * correspond to an output port.
   */
  template <class T> void setOutput(const std::string &portName, const T &obj) {
    setOutputInternal(portName, obj);
  }

  /**
   * @brief Check whether a specified port has got any input.
   *
   * @param portName The name of the port.
   * @return true if input is available. Get it with @ref getInput()
   * @return false if no input is available.
   */
  bool hasInput(const std::string &portName);
  /**
   * @brief Get an object as input of the node from a specified input port.
   *
   * @tparam T The type of the object that is retrieved.
   * @param portName The name of the input port.
   * @param obj The object which is returned.
   */
  template <class T> void getInput(const std::string &portName, T &obj) {

    try {
      std::any anyObj;
      getInputInternal(portName, anyObj);
      obj = std::any_cast<T>(anyObj);
    } catch (const std::bad_any_cast &bac) {

      std::string msg = "Error in node '" + name_ + "' of type '" + type_ +
                        "' when getting object from input on port '" +
                        portName +
                        "'. This error might come from a wrong typename in "
                        "the template argument. Additional information: " +
                        std::string(bac.what());
      reportError(msg);
    } catch (const std::exception &ex) {

      std::string msg = "Error in node '" + name_ + "' of type '" + type_ +
                        "' when getting object from input on port '" +
                        portName +
                        "'. Additional information: " + std::string(ex.what());

      reportError(msg);
    }
  }

  /**
   * @brief Get the setting using the key.
   *
   * @tparam T The type of the setting object stored under the specified key. T
   * must be default constructible.
   * @param key The key to access the setting.
   * @return T The setting object.
   */
  template <class T> T getSetting(const std::string &key) {

    if (settings_.find(key) == settings_.end()) {
      std::string msg = "Error when getting setting for node '" + name_ +
                        "' of type '" + type_ +
                        "'. There is no setting with key '" + key + "'.";
      reportError(msg);
    }
    try {
      return settings_[key].get<T>();
    } catch (const std::exception &ex) {

      std::string msg = "Error when getting setting for node '" + name_ +
                        "' of type '" + type_ + "': " + std::string(ex.what());

      reportError(msg);
    }
    return T();
  }
  /**
   * @brief Get a list of all input port names. This can be useful if the node
   * shall expected a dynamic range of inputs.
   *
   * @return std::set<std::string> The input port names.
   */
  std::set<std::string> getInputPortNames();
  /**
   * @brief Get a list of all output port names.
   *
   * @return std::set<std::string> The output port names.
   */
  std::set<std::string> getOutputPortNames();
  /**
   * @brief Get a list of all port names with the specified port type.
   *
   * @param type The type of the port. Can be INPUT or OUTPUT.
   * @return std::set<std::string> The port names which match the specified
   * type.
   */
  std::set<std::string> getPortNames(PortType type);

private:
  // Stores the name of the node
  std::string name_;
  // Stores the type of the node
  std::string type_;
  // The settings of the node as a nlohmann::json object. This allows each node
  // to use any data structure as settings during construction.
  nlohmann::json settings_;
  // The connector object is shared among all nodes.
  std::shared_ptr<Connector> connector_;

  void setOutputInternal(const std::string &portName, const std::any &anyObj);
  void getInputInternal(const std::string &portName, std::any &anyObj);
  void reportError(const std::string &msg);
};

/**
 * @brief Convenience typedef for a unique_pointer of an abstract Node.
 */
typedef std::unique_ptr<Node> NodePtr;

/**
 * @brief Templated function for instantiating a concrete node and returning a
 * pointer to an abstract Node.
 *
 * @tparam DerivedNodeType The type name of the concrete node.
 * @param nodeDefinition  The node definition that holds the settings the name
 * and the type of the node.
 * @param connector The node connector used for connecting the nodes with
 * eachother.
 * @return NodePtr Pointer to an abstract node.
 */
template <class DerivedNodeType>
NodePtr createNode(const NodeDefinition &nodeDefinition,
                   const std::shared_ptr<Connector> &connector) {
  return std::make_unique<DerivedNodeType>(
      DerivedNodeType(nodeDefinition, connector));
}

} // namespace execution_nodes