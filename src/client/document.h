#ifndef DOCUMENT_H
#define DOCUMENT_H
#include "stdlib.h"
#include <string>
#include <list>
#include "account.h"

using namespace std;


class Document
{
public:
    Document();
private:
    string name;
    string path;
    string id;
    list<Account> users_authorized;
    list<Account> users_aut_online;
};

#endif // DOCUMENT_H
