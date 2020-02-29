#include "user.h"
#include "stdlib.h"
#include <string>
#include <list>


using namespace std;


User::User(string nickname, string email)
{
    this->email=email;
    this->nickname=nickname;
}

User::~User()
{
    delete this;
}

Cursor *User::getCursor() const
{
    return cursor;
}

void User::setCursor(Cursor *value)
{
    cursor = value;
}

