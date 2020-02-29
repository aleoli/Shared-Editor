#ifndef DOCUMENT_H
#define DOCUMENT_H
#include "stdlib.h"
#include <string>
#include <QWidget>
#include <list>

using namespace std;
//class User;

class Document
{
public:
    Document();
    //list<User> getUsers();
    //void setUsers(list<User> users){ online_users.splice(online_users.end(), users);}
    string getName() const;
    void setName(const string &value);
    string getPath() const;
    void setPath(const string &value);
    string getId() const;
    void setId(const string &value);

private:
    string name;
    string path;
    string id;
    //list<User> online_users;
    list<QWidget> users_cursors; //cursors of online users
};

#endif // DOCUMENT_H
