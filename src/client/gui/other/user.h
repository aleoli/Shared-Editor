#ifndef USER_H
#define USER_H
#include "stdlib.h"
#include <string>
#include <list>
#include <QWidget>

using namespace std;
class Document;
class Cursor;

class User
{
public:
    User(string nickname, string email);
    virtual ~User();
    string getEmail(){return email;}
    string getNickname(){return nickname;}
    void setEmail(const string &value);
    void setNickname(const string &value);
    Cursor *getCursor() const;
    void setCursor(Cursor *value);

private:
    string email;
    string nickname;
    Document *doc;
    Cursor *cursor;

};

#endif // USER_H
