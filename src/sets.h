#pragma once

#include <memory>

#include <QString>

class Sets {
public:
  typedef enum LogLevel {
    ERROR, WARN, DEBUG, INFO
  } LogLevel;

  Sets(const Sets&) = delete;
  Sets(Sets&&) = delete;
  Sets& operator=(const Sets&) = delete;
  Sets() = delete;
  Sets& operator=(Sets&&) = delete;

  static std::shared_ptr<Sets> get(QString log_level = "error");

  ~Sets();

  const Sets::LogLevel& getLogLevel() const;

private:
  static std::shared_ptr<Sets> instance;
  explicit Sets(LogLevel log_level);

  static LogLevel getLogLevel(QString log_level);

  LogLevel _log_level;
};
