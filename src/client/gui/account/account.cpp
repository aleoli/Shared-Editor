#include "account.h"
#include <QString>

using namespace std;

Account::Account(QString username, QString password)
{
    this->email=username;
    this->password=password;
}

/*
void Account::setName(const QString &value)
{
    name = value;
}

void Account::setSurname(const QString &value)
{
    surname = value;
}
*/
void Account::setServerResponse(QString token, int userId, QString nickname, QString icon)
{
    this->token = token;
    this->userId = userId;
    this->nickname = nickname;
    this->icon = icon;
}
void Account::setEmail(const QString &value)
{
    email = value;
}

void Account::setNickname(const QString &value)
{
    nickname = value;
}

void Account::setPassword(const QString &value)
{
    password = value;
}

const QString &Account::getToken() const {
    return token;
}

void Account::setToken(const QString &token) {
    Account::token = token;
}
/*
void Account::setBirthdate(const QString &value)
{
    birthdate = value;
}



void Account::setSex(const QString &value)
{
    sex = value;
}

*/
