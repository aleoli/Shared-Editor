#ifndef ACCOUNT_H
#define ACCOUNT_H
#include "stdlib.h"
#include <QString>
#include <list>
//#include <document.h>

using namespace std;

class Account
{
public:
    Account(QString user, QString password);
    void setServerResponse(QString token, int userId, QString nickname, QString icon);
    //QString getName(){return name;}
    //QString getSurname(){return surname;}
    QString getEmail(){return email;}
    QString getNickname(){return nickname;}
    //QString getBirthdate(){return birthdate;}
    QString getPassword(){return password;}
    //QString getSex(){return sex;}

    //void setName(const QString &value);
    //void setSurname(const QString &value);
    void setEmail(const QString &value);
    void setNickname(const QString &value);
    //void setBirthdate(const QString &value);
    void setPassword(const QString &value);
    //void setSex(const QString &value);

private:
    //QString name;
    //QString surname;
    QString email;
    QString nickname;
    //QString birthdate;
    QString password;
    int userId;
    QString token;
public:
    const QString &getToken() const;

    void setToken(const QString &token);

private:
    // token di sessione
    QString icon;
    //QString sex;
    //list<Document> accessible_documents;
};

#endif // ACCOUNT_H
