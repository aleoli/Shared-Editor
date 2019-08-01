#pragma once

#include <QString>
#include <QObject>
#include <QSqlTableModel>
#include <QSqlRecord>

#include <vector>

#include "persistence_global.h"
#include "dependencies.h"

using persistence::Persistent;
using persistence::LazyList;

class Employee: public Persistent {
public:
    Employee(const Employee &e);
    Employee(Employee &&e);
    explicit Employee();
    explicit Employee(QSqlRecord r);

    virtual ~Employee();

    virtual Employee& operator=(const Employee& e);

    QString getName() const;
    void setName(QString name);
    int getSalary() const;
    void setSalary(int salary);
    std::vector<Project*> getProjects();

    virtual void save();
    virtual void save_record(QSqlRecord &r);
    virtual void remove();

    const static QString table_name;

private:
    QString name;
    int salary;
    LazyList<Project> projects;
};
