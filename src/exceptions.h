#include <utility>

#pragma once

#include <QString>

namespace se_exceptions {
  // parent class
  class SE_Exception: public std::exception {
  public:
      explicit SE_Exception(QString str): std::exception(), str(std::move(str)) {
        this->std_str = this->str.toStdString();
      }

      [[nodiscard]] const char* what() const noexcept override {
          return this->std_str.c_str();
      }

  private:
      QString str;
      std::string std_str;
  };

  // arg parse exceptions
  class ArgNotValidException: public SE_Exception {
  public:
      explicit ArgNotValidException(QString str): SE_Exception(std::move(str)) {}
  };

  // database exceptions
  class SQLException: public SE_Exception {
  public:
      explicit SQLException(QString str): SE_Exception(std::move(str)) {}
  };

  class SQLInsertException: public SQLException {
  public:
      explicit SQLInsertException(QString str): SQLException(std::move(str)) {}
  };

  class SQLNoElementException: public SQLException {
  public:
      explicit SQLNoElementException(QString str): SQLException(std::move(str)) {}
  };

  class SQLNoElementSelectException: public SQLNoElementException {
  public:
      explicit SQLNoElementSelectException(QString str): SQLNoElementException(std::move(str)) {}
  };

  class SQLNoElementUpdateException: public SQLNoElementException {
  public:
      explicit SQLNoElementUpdateException(QString str): SQLNoElementException(std::move(str)) {}
  };

  class SQLNoElementDeleteException: public SQLNoElementException {
  public:
      explicit SQLNoElementDeleteException(QString str): SQLNoElementException(std::move(str)) {}
  };

  // Message exceptions
  class MessageFromJsonException : public SE_Exception {
  public:
      explicit MessageFromJsonException(QString str): SE_Exception(std::move(str)) {}
  };

  class MessageDataException : public SE_Exception {
  public:
      explicit MessageDataException(QString str): SE_Exception(std::move(str)) {}
  };

  // File related exceptions
  class SymbolIdFromJsonException : public SE_Exception {
  public:
      explicit SymbolIdFromJsonException(QString str): SE_Exception(std::move(str)) {}
  };

  class SymbolFromJsonException : public SE_Exception {
  public:
      explicit SymbolFromJsonException(QString str): SE_Exception(std::move(str)) {}
  };

  class SymbolIdentifierFromJsonException : public SymbolFromJsonException {
  public:
      explicit SymbolIdentifierFromJsonException(QString str): SymbolFromJsonException(std::move(str)) {}
  };

  class FileException : public SE_Exception {
  public:
      explicit FileException(QString str): SE_Exception(std::move(str)) {}
  };

  class FileFromJsonException : public FileException {
  public:
      explicit FileFromJsonException(QString str): FileException(std::move(str)) {}
  };

  class FileLocalInsertException : public FileException {
  public:
      explicit FileLocalInsertException(QString str): FileException(std::move(str)) {}
  };

  class FileSymbolsException : public FileException {
  public:
      explicit FileSymbolsException(QString str): FileException(std::move(str)) {}
  };

  class FileUserException : public FileException {
  public:
      explicit FileUserException(QString str): FileException(std::move(str)) {}
  };

  // MessageManagerExceptions
  class MessageManagerException : public SE_Exception {
  public:
      explicit MessageManagerException(QString str): SE_Exception(std::move(str)) {}
  };

  class ClientLoginException : public MessageManagerException {
  public:
      explicit ClientLoginException(QString str): MessageManagerException(std::move(str)) {}
  };

  class ClientMultipleLoginException : public MessageManagerException {
  public:
      explicit ClientMultipleLoginException(QString str): MessageManagerException(std::move(str)) {}
  };

  class ClientFileException : public MessageManagerException {
  public:
      explicit ClientFileException(QString str): MessageManagerException(std::move(str)) {}
  };

  class FileNotFoundException : public MessageManagerException {
  public:
      explicit FileNotFoundException(QString str): MessageManagerException(std::move(str)) {}
  };

  // Comment exceptions
  class CommentException : public SE_Exception {
  public:
      explicit CommentException(QString str): SE_Exception(std::move(str)) {}
  };

  // MainThread exceptions
  class MainThreadException : public SE_Exception {
  public:
      explicit MainThreadException(QString str): SE_Exception(std::move(str)) {}
  };

  class FileNotReceivedException : public MainThreadException {
  public:
      explicit FileNotReceivedException(QString str): MainThreadException(std::move(str)) {}
  };

  // FSElement exceptions
  class FSElementFromJsonException : public SE_Exception {
  public:
      explicit FSElementFromJsonException(QString str): SE_Exception(std::move(str)) {}
  };

  // test exception
  class TestException: public SE_Exception {
  public:
      explicit TestException(QString str): SE_Exception(std::move(str)) {}
  };

  // File System exceptions
  class FileSystemException: public SE_Exception {
  public:
      explicit FileSystemException(QString str): SE_Exception(std::move(str)) {}
  };

  class IllegalAccessException: public FileSystemException {
  public:
      explicit IllegalAccessException(QString str): FileSystemException(std::move(str)) {}
  };

  class RootDirException: public FileSystemException {
  public:
      explicit RootDirException(QString str): FileSystemException(std::move(str)) {}
  };

  class NoFileOnDiskException: public FileSystemException {
  public:
      explicit NoFileOnDiskException(QString str): FileSystemException(std::move(str)) {}
  };

  class LinkException: public FileSystemException {
  public:
      explicit LinkException(QString str): FileSystemException(std::move(str)) {}
  };

  class DoubleLinkException: public FileSystemException {
  public:
      explicit DoubleLinkException(QString str): FileSystemException(std::move(str)) {}
  };

  class ReadException: public IllegalAccessException {
  public:
      explicit ReadException(QString str): IllegalAccessException(std::move(str)) {}
  };

  class WriteException: public IllegalAccessException {
  public:
      explicit WriteException(QString str): IllegalAccessException(std::move(str)) {}
  };

  class ShareException: public FileSystemException {
  public:
      explicit ShareException(QString str): FileSystemException(std::move(str)) {}
  };


  // Gui exceptions
  class GuiException : public SE_Exception {
  public:
      GuiException(QString str): SE_Exception(std::move(str)) {}
  };

  class ImageException : public GuiException {
  public:
      ImageException(QString str): GuiException(std::move(str)) {}
  };

  class UserException : public SE_Exception {
  public:
      UserException(QString str): SE_Exception(std::move(str)) {}
  };
}
