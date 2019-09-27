#pragma once

namespace se_exceptions {
  // parent class
  class SE_Exception: public std::exception {
  public:
      SE_Exception(QString str): std::exception(), str(str) {
        this->std_str = this->str.toStdString();
      }

      virtual const char* what() const noexcept {
          return this->std_str.c_str();
      }

  private:
      QString str;
      std::string std_str;
  };

  // arg parse exceptions
  class ArgNotValidException: public SE_Exception {
  public:
      ArgNotValidException(QString str): SE_Exception(str) {}
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

  // File related exceptions
  class SymbolIdFromJsonException : public SE_Exception {
  public:
      SymbolIdFromJsonException(QString str): SE_Exception(str) {}
  };

  class SymbolFromJsonException : public SE_Exception {
  public:
      SymbolFromJsonException(QString str): SE_Exception(str) {}
  };

  class SymbolIdentifierFromJsonException : public SymbolFromJsonException {
  public:
      SymbolIdentifierFromJsonException(QString str): SymbolFromJsonException(str) {}
  };

  class FileException : public SE_Exception {
  public:
      FileException(QString str): SE_Exception(str) {}
  };

  class FileFromJsonException : public FileException {
  public:
      FileFromJsonException(QString str): FileException(str) {}
  };

  class FileLocalInsertException : public FileException {
  public:
      FileLocalInsertException(QString str): FileException(str) {}
  };

  class FileSymbolsException : public FileException {
  public:
      FileSymbolsException(QString str): FileException(str) {}
  };

  // MessageManagerExceptions
  class MessageManagerException : public SE_Exception {
  public:
      MessageManagerException(QString str): SE_Exception(str) {}
  };

  class ClientLoginException : public MessageManagerException {
  public:
      ClientLoginException(QString str): MessageManagerException(str) {}
  };

  class ClientFileException : public MessageManagerException {
  public:
      ClientFileException(QString str): MessageManagerException(str) {}
  };

  class FileNotFoundException : public MessageManagerException {
  public:
      FileNotFoundException(QString str): MessageManagerException(str) {}
  };
}
