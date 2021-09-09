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

/**
 * @brief Defines the logging levels of the logger.
 *
 */
enum class LogLevel : uint8_t {
  LVL_ERROR,
  LVL_WARNING,
  LVL_INFO,
  LVL_DEBUG,
};

/**
 * @brief A very basic logger class. I recommend only using it internally.
 */
class Log {
public:
  Log();
  virtual ~Log();
  std::ostringstream &ErrorThrow(const std::string &functionName = "");
  std::ostringstream &Error(const std::string &functionName = "",
                            bool throwException = false);
  std::ostringstream &Warning(const std::string &functionName = "");
  std::ostringstream &Info(const std::string &functionName = "");
  std::ostringstream &Debug(const std::string &functionName = "");

public:
  static LogLevel &getLogLevel();
  static void setLogLevel(LogLevel level);

private:
  std::ostringstream &InternalLog(LogLevel level,
                                  const std::string &functionName = "",
                                  bool throwException = false);

  bool doThrow;
  std::ostringstream os;
  LogLevel messageLevel;

  Log(const Log &);
  Log &operator=(const Log &) = delete;
  static LogLevel logginglevel;
};

/**
 * @brief Write a log message on the info channel.
 */
#define LOG_INFO Log().Info(__func__)
/**
 * @brief Write a log message on the error channel. Writes to std::cerr.
 */
#define LOG_ERROR Log().Error(__func__)
/**
 * @brief Write a log message on the error channel. Writes to std::cerr and
 * throws a std::runtime_error.
 */
#define THROW_ERROR Log().ErrorThrow(__func__)
/**
 * @brief Write a log message on the debug channel.
 */
#define LOG_DEBUG Log().Debug(__func__)
/**
 * @brief Write a log message on the warning channel.
 */
#define LOG_WARNING Log().Warning(__func__)

} // namespace execution_nodes