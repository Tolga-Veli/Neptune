#pragma once

#include <cstdint>
#include <format>
#include <iostream>
#include <string_view>

#include "Utilities.hpp"

namespace Neptune::Core {

enum class LogLevel : uint16_t { Trace = 0, Info, Warning, Error, Fatal, None };

class Logger {
public:
  static inline constexpr std::string_view NEPTUNE_COL_GRAY = "\033[90m";
  static inline constexpr std::string_view NEPTUNE_COL_GREEN = "\033[32m";
  static inline constexpr std::string_view NEPTUNE_COL_YELLOW = "\033[33m";
  static inline constexpr std::string_view NEPTUNE_COL_RED = "\033[31m";
  static inline constexpr std::string_view NEPTUNE_COL_FATAL =
      "\033[41m\033[37m";

  static void SetLevel(LogLevel level) { s_currentLevel = level; }
  static LogLevel GetLevel() { return s_currentLevel; }

  template <typename... Args>
  static void Log(LogLevel level, std::string_view prefix,
                  std::string_view color, std::format_string<Args...> fmt,
                  Args &&...args) {
    if (level < s_currentLevel)
      return;

    std::string message = std::format(fmt, std::forward<Args>(args)...);
    std::cout << color << "[" << prefix << "] " << message << "\033[0m"
              << std::endl;
  }

  template <typename... Args>
  static void LogFileAndLine(LogLevel level, std::string_view prefix,
                             std::string_view color, std::string_view file,
                             i32 line, std::format_string<Args...> fmt,
                             Args &&...args) {
    if (level < s_currentLevel)
      return;

    std::string message = std::format(fmt, std::forward<Args>(args)...);
    std::cout << color << "[" << prefix << "] " << file << ":" << line << ": "
              << message << "\033[0m" << std::endl;
  }

private:
  static inline LogLevel s_currentLevel = LogLevel::Trace;
};

} // namespace Neptune::Core

#define NEPTUNE_DEBUG 1

#ifdef NEPTUNE_DEBUG
#define NEPTUNE_INFO(...)                                                      \
  ::Neptune::Core::Logger::Log(::Neptune::Core::LogLevel::Info, "INFO",        \
                               ::Neptune::Core::Logger::NEPTUNE_COL_GREEN,     \
                               __VA_ARGS__)
#define NEPTUNE_WARN(...)                                                      \
  ::Neptune::Core::Logger::Log(::Neptune::Core::LogLevel::Warning, "WARN",     \
                               ::Neptune::Core::Logger::NEPTUNE_COL_YELLOW,    \
                               __VA_ARGS__)
#else
#define NEPTUNE_INFO(...)
#define NEPTUNE_WARN(...)
#endif

#define NEPTUNE_ERROR(...)                                                     \
  ::Neptune::Core::Logger::LogFileAndLine(                                     \
      ::Neptune::Core::LogLevel::Error, "ERROR",                               \
      ::Neptune::Core::Logger::NEPTUNE_COL_RED,                                \
      ::Neptune::Core::strip_path(__FILE__), __LINE__, __VA_ARGS__)

#define NEPTUNE_FATAL(...)                                                     \
  do {                                                                         \
    ::Neptune::Core::Logger::LogFileAndLine(                                   \
        ::Neptune::Core::LogLevel::Fatal, "FATAL",                             \
        ::Neptune::Core::Logger::NEPTUNE_COL_FATAL,                            \
        ::Neptune::Core::strip_path(__FILE__), __LINE__, __VA_ARGS__);         \
    std::abort();                                                              \
  } while (0)
