#include <execution_nodes/helpers.h>
#include <sstream>
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

} // namespace execution_nodes