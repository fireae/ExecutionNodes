// log.cpp
#include <execution_nodes/logging.hpp>
#include <iostream>
#include <spdlog/spdlog.h>
#include <strstream>

namespace execution_nodes {

    /*
struct Log::Impl {};

std::string convert(std::vector<std::function<void(std::strstream &)>> v) {
  std::strstream sstr;
  for (auto &&i : v) {
    i(sstr);
  }
  return sstr.str();
}

void Log::errorImpl(std::vector<std::function<void(std::strstream &)>> v) {
  spdlog::error(convert(v));
}

void Log::warningImpl(std::vector<std::function<void(std::strstream &)>> v) {
  spdlog::warn(convert(v));
}

void Log::throwErrorImpl(std::vector<std::function<void(std::strstream &)>> v) {

  auto msg = convert(v);
  spdlog::error(msg);
  throw std::runtime_error(msg);
}

std::unique_ptr<Log::Impl> Log::impl_ =
    std::make_unique<Log::Impl>(Log::Impl());
    */

} // namespace execution_nodes