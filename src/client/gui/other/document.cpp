#include "document.h"

Document::Document()
{

}

string Document::getName() const
{
    return name;
}

void Document::setName(const string &value)
{
    name = value;
}

string Document::getPath() const
{
    return path;
}

void Document::setPath(const string &value)
{
    path = value;
}

string Document::getId() const
{
    return id;
}

void Document::setId(const string &value)
{
    id = value;
}

