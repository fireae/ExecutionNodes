// log.cpp
#include <chrono>
#include <execution_nodes/internal/logging.hpp>
#include <iostream>
#include <map>

namespace execution_nodes {

LogLevel Log::logginglevel = LogLevel::LVL_ERROR;
Log::Log() : doThrow(false) {}

static const std::map<uint8_t, std::string> levelToString = {
    {static_cast<uint8_t>(LogLevel::LVL_DEBUG), "Debug"},
    {static_cast<uint8_t>(LogLevel::LVL_ERROR), "Error"},
    {static_cast<uint8_t>(LogLevel::LVL_INFO), "Info"},
    {static_cast<uint8_t>(LogLevel::LVL_WARNING), "Warning"},
};

LogLevel &Log::getLogLevel() { return logginglevel; }
void Log::setLogLevel(LogLevel level) { logginglevel = level; }

std::ostringstream &Log::InternalLog(LogLevel level,
                                     const std::string &functionName,
                                     bool throwException) {
  os << "[" << levelToString.at(static_cast<uint8_t>(level)) << "]";
  if (functionName != "") {
    os << " in " << functionName << "():";
  }
  os << "\t";
  messageLevel = LogLevel::LVL_ERROR;
  doThrow = throwException;
  return os;
}

std::ostringstream &Log::ErrorThrow(const std::string &functionName) {
  return InternalLog(LogLevel::LVL_ERROR, functionName, true);
}

std::ostringstream &Log::Error(const std::string &functionName,
                               bool throwException) {
  return InternalLog(LogLevel::LVL_ERROR, functionName, throwException);
}

std::ostringstream &Log::Warning(const std::string &functionName) {
  return InternalLog(LogLevel::LVL_WARNING, functionName, false);
}

std::ostringstream &Log::Info(const std::string &functionName) {
  return InternalLog(LogLevel::LVL_INFO, functionName, false);
}

std::ostringstream &Log::Debug(const std::string &functionName) {
  return InternalLog(LogLevel::LVL_DEBUG, functionName, false);
}

Log::~Log() noexcept(false) {
  if (messageLevel >= Log::getLogLevel()) {
    os << std::endl;
    std::string msg = os.str();

    if (messageLevel > LogLevel::LVL_ERROR) {
      std::cerr << msg;
    } else {

      std::cout << msg;
    }

    if (doThrow) {
      throw std::runtime_error(msg);
    }
  }
}

} // namespace execution_nodes