#include <execution_nodes/internal/helpers.h>
#include <sstream>
#include <execution_nodes/internal/logging.h>

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

size_t getIndexOfElement(const std::string &element,
                         const std::vector<std::string> &list, bool doThrow) {
  for (size_t i = 0; i < list.size(); i++) {
    if (list[i] == element) {
      return i;
    }
  }
  if (doThrow) {
    THROW_ERROR << "Error when sorting nodes. Unable to find node '" << element
                << "' in the list of connections";
  }
  return list.size() + 1;
}


} // namespace execution_nodes