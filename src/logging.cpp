// log.cpp
#include <execution_nodes/logging.hpp>
#include <iostream>
#include <map>
#include <chrono>

namespace execution_nodes {

LogLevel Log::logginglevel = LogLevel::LVL_ERROR;
Log::Log() : doThrow(false) {}


std::string time_in_HH_MM_SS_MMM() {
  namespace sc = std::chrono;

  // get current time
  auto now = sc::system_clock::now();

  // get number of milliseconds for the current second
  // (remainder after division into seconds)
  auto ms = sc::duration_cast<sc::milliseconds>(now.time_since_epoch()) % 1000;

  // convert to std::time_t in order to convert to std::tm (broken time)
  auto timer = sc::system_clock::to_time_t(now);

  // convert to broken time
  std::tm bt = *std::localtime(&timer);

  std::ostringstream oss;

  oss << std::put_time(&bt, "%H:%M:%S"); // HH:MM:SS
  oss << '.' << std::setfill('0') << std::setw(3) << ms.count();

  return oss.str();
}

std::string getTimestamp() {
  return time_in_HH_MM_SS_MMM();
  //auto t = std::time(nullptr);
  //auto tm = *std::localtime(&t);

  //std::ostringstream oss;
  //oss.clear();
  //oss << std::put_time(&tm, "%H:%M:%S");
  //auto str = oss.str();
  //return str;
}

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
  os << getTimestamp();
  os << " "
     << "[" << levelToString.at(static_cast<uint8_t>(level)) << "]";
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

Log::~Log() {
  if (messageLevel >= Log::getLogLevel()) {
    os << std::endl;
    std::string msg = os.str();

    std::cout << msg;

    if (doThrow) {
      throw std::runtime_error(msg);
    }
  }
}

} // namespace execution_nodes