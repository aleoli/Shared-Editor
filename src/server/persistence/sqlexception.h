#ifndef SQLEXCEPTION_H
#define SQLEXCEPTION_H

#include "persistence.h"

#include <exception>

class persistence::SQLException: public std::exception {
public:
    SQLException(QString str): std::exception(), str(str) {}

    virtual const char* what() const noexcept {
        return this->str.toStdString().c_str();
    }

private:
    QString str;
};


class persistence::SQLInsertException: public persistence::SQLException {
public:
    SQLInsertException(QString str): SQLException(str) {}
};

class persistence::SQLNoElementException: public persistence::SQLException {
public:
    SQLNoElementException(QString str): SQLException(str) {}
};

class persistence::SQLNoElementSelectException: public persistence::SQLNoElementException {
public:
    SQLNoElementSelectException(QString str): SQLNoElementException(str) {}
};

class persistence::SQLNoElementUpdateException: public persistence::SQLNoElementException {
public:
    SQLNoElementUpdateException(QString str): SQLNoElementException(str) {}
};

class persistence::SQLNoElementDeleteException: public persistence::SQLNoElementException {
public:
    SQLNoElementDeleteException(QString str): SQLNoElementException(str) {}
};

#endif // SQLEXCEPTION_H
