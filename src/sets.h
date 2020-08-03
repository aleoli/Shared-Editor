#pragma once

#include <memory>

#include <QString>

class Sets {
public:
  typedef enum LogLevel {
    NONE, ERROR, WARN, INFO, DEBUG
  } LogLevel;

  Sets(const Sets&) = delete;
  Sets(Sets&&) = delete;
  Sets& operator=(const Sets&) = delete;
  Sets() = delete;
  Sets& operator=(Sets&&) = delete;

  static std::shared_ptr<Sets> get(const QString& log_level = "none");

  ~Sets();

  [[nodiscard]] const Sets::LogLevel& getLogLevel() const;

private:
  static std::shared_ptr<Sets> instance;
  explicit Sets(LogLevel log_level);

  static LogLevel getLogLevel(const QString& log_level);

  LogLevel _log_level;
};
