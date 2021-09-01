// log.cpp
#include <execution_nodes/logging.hpp>
#include <iostream>

namespace execution_nodes {

LogLevel Log::logginglevel = LogLevel::LVL_ERROR;
Log::Log() : doThrow(false) {}

std::string getTimestamp() {
  auto t = std::time(nullptr);
  auto tm = *std::localtime(&t);

  std::ostringstream oss;
  oss.clear();
  oss << std::put_time(&tm, "%d-%m-%Y %H-%M-%S");
  auto str = oss.str();
  return str;
}

LogLevel &Log::getLogLevel() { return logginglevel; }
void Log::setLogLevel(LogLevel level) { logginglevel = level; }

std::ostringstream &Log::ErrorThrow() {
  os << getTimestamp();
  os << " "
     << "[Error]"
     << ": ";
  messageLevel = LogLevel::LVL_ERROR;
  doThrow = true;
  return os;
}

std::ostringstream &Log::Error(bool throwException) {
  os << getTimestamp();
  os << " "
     << "[Error]"
     << ": ";
  messageLevel = LogLevel::LVL_ERROR;
  doThrow = throwException;
  return os;
}

std::ostringstream &Log::Warning() {
  os << getTimestamp();
  os << " "
     << "[Warning]"
     << ": ";
  messageLevel = LogLevel::LVL_WARNING;
  return os;
}

std::ostringstream &Log::Info() {
  os << getTimestamp();
  os << " "
     << "[Info]"
     << ": ";
  messageLevel = LogLevel::LVL_INFO;
  return os;
}

std::ostringstream &Log::Debug() {
  os << getTimestamp();
  os << " "
     << "[Debug]"
     << ": ";
  messageLevel = LogLevel::LVL_DEBUG;
  return os;
}

Log::~Log() {
  if (messageLevel >= Log::getLogLevel()) {
    os << std::endl;
    std::string msg = os.str();

    std::cout << msg;

    if (doThrow) {
      throw std::runtime_error(msg);
    }

    // fprintf(stderr, "%s", os.str().c_str());
    // fflush(stderr);
  }
}

} // namespace execution_nodes