#ifndef ACCOUNT_H
#define ACCOUNT_H
#include "stdlib.h"
#include <string>
#include <list>
//#include <document.h>

using namespace std;

class Account
{
public:
    Account();
    string getName(){return name;}
    string getSurname(){return surname;}
    string getEmail(){return email;}
    string getNickname(){return nickname;}
    string getBirthdate(){return birthdate;}
    string getPassword(){return password;}
    string getSex(){return sex;}

    void setName(const string &value);
    void setSurname(const string &value);
    void setEmail(const string &value);
    void setNickname(const string &value);
    void setBirthdate(const string &value);
    void setPassword(const string &value);
    void setSex(const string &value);

private:
    string name;
    string surname;
    string email;
    string nickname;
    string birthdate;
    string password;
    string sex;
    //list<Document> accessible_documents;
};

#endif // ACCOUNT_H
