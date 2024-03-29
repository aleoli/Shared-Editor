#include "sets.h"

#include "exceptions.h"

std::shared_ptr<Sets> Sets::instance = nullptr;

Sets::Sets(Sets::LogLevel log_level): _log_level(log_level) {
}

Sets::~Sets() = default;

std::shared_ptr<Sets> Sets::get(const QString& log_level) {
  if(instance == nullptr) {
    instance.reset(new Sets{Sets::getLogLevel(log_level)});
  }
  return instance;
}

Sets::LogLevel Sets::getLogLevel(const QString& log_level) {
  if(log_level == "none") {
    return Sets::LogLevel::NONE;
  } else if(log_level == "error") {
    return Sets::LogLevel::ERROR;
  } else if(log_level == "warn") {
    return Sets::LogLevel::WARN;
  } else if(log_level == "info") {
    return Sets::LogLevel::INFO;
  } else if(log_level == "debug") {
    return Sets::LogLevel::DEBUG;
  }
  throw se_exceptions::ArgNotValidException{"Not valid value for log level '"+log_level+"'"};
}

const Sets::LogLevel& Sets::getLogLevel() const {
  return this->_log_level;
}
