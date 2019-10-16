#include "account.h"
#include "string.h"

using namespace std;

Account::Account()
{

}

void Account::setName(const string &value)
{
    name = value;
}

void Account::setSurname(const string &value)
{
    surname = value;
}

void Account::setEmail(const string &value)
{
    email = value;
}

void Account::setNickname(const string &value)
{
    nickname = value;
}

void Account::setBirthdate(const string &value)
{
    birthdate = value;
}

void Account::setPassword(const string &value)
{
    password = value;
}

void Account::setSex(const string &value)
{
    sex = value;
}


