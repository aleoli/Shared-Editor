#pragma once

namespace se_exceptions {
  // parent class
  class SE_Exception: public std::exception {
  public:
      SE_Exception(QString str): std::exception(), str(str) {
        this->c_str = new char[str.length()+1];
        int i=0;
        for(auto& c: str.toStdString()) {
          this->c_str[i++] = c;
        }
        this->c_str[i] = '\0';
      }

      virtual ~SE_Exception() {
        delete[] this->c_str;
        this->c_str = nullptr;
      }

      virtual const char* what() const noexcept {
          return this->c_str;
      }

  private:
      QString str;
      char *c_str;
  };

  // database exceptions
  class SQLException: public SE_Exception {
  public:
      SQLException(QString str): SE_Exception(str) {}
  };

  class SQLInsertException: public SQLException {
  public:
      SQLInsertException(QString str): SQLException(str) {}
  };

  class SQLNoElementException: public SQLException {
  public:
      SQLNoElementException(QString str): SQLException(str) {}
  };

  class SQLNoElementSelectException: public SQLNoElementException {
  public:
      SQLNoElementSelectException(QString str): SQLNoElementException(str) {}
  };

  class SQLNoElementUpdateException: public SQLNoElementException {
  public:
      SQLNoElementUpdateException(QString str): SQLNoElementException(str) {}
  };

  class SQLNoElementDeleteException: public SQLNoElementException {
  public:
      SQLNoElementDeleteException(QString str): SQLNoElementException(str) {}
  };

  // Message exceptions
  class MessageFromJsonException : public SE_Exception {
  public:
      MessageFromJsonException(QString str): SE_Exception(str) {}
  };

}
