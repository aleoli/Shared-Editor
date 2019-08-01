#pragma once

#include <QObject>

#include "persistence_global.h"
#include "dependencies.h"

using persistence::Persistent;
using persistence::Lazy;

class Project : public Persistent {
public:
    explicit Project();
    Project(const Project &e);
    Project(Project &&e);
    explicit Project(QSqlRecord r);

    virtual ~Project();

    virtual Project& operator=(const Project& e);

    virtual void save();
    virtual void save_record(QSqlRecord &r);
    virtual void remove();

    const static QString table_name;

    QString getName() const;
    int getEmployeeId() const;
    Employee getEmployee();

private:
    QString name;
    Lazy<Employee> employee;
};
