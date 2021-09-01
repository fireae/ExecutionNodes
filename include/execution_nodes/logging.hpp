#pragma once

#include <ctime>
#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <utility>
#include <vector>

namespace execution_nodes {

enum class LogLevel : uint8_t {
  LVL_ERROR,
  LVL_WARNING,
  LVL_INFO,
  LVL_DEBUG,
};

class Log {
public:
  Log();
  virtual ~Log();
  std::ostringstream &ErrorThrow();
  std::ostringstream &Error(bool throwException = false);
  std::ostringstream &Warning();
  std::ostringstream &Info();
  std::ostringstream &Debug();

public:
  static LogLevel &getLogLevel();
  static void setLogLevel(LogLevel level);

protected:
  bool doThrow;
  std::ostringstream os;
  LogLevel messageLevel;

private:
  Log(const Log &);
  Log &operator=(const Log &) = delete;
  static LogLevel logginglevel;
};

} // namespace execution_nodes