#include "employee.h"

#include <QDebug>
#include <QSqlTableModel>

#include "project.h"

const QString Employee::table_name = "employee";

Employee::Employee(): Persistent(), name(""), salary(0) {}

Employee::Employee(const Employee &e): Persistent(e) {
    this->name = e.name;
    this->salary = e.salary;
    this->projects = e.projects;
}

Employee::Employee(Employee &&e): Persistent(e) {
    this->name = e.name;
    this->salary = e.salary;
    this->projects = e.projects;
}

Employee::Employee(QSqlRecord r): Persistent(r) {
    this->name = r.value("name").toString();
    this->salary = r.value("salary").toInt();
    this->projects = LazyList<Project>("project", "employee = "+QString::number(this->id));
}

Employee::~Employee() {}

Employee& Employee::operator=(const Employee &e) {
    if(this == &e) {
        return *this;
    }
    Persistent::operator=(e);
    this->name = e.name;
    this->salary = e.salary;
    this->projects = e.projects;
    return *this;
}

void Employee::save() {
    this->_save<Employee>();
}

void Employee::save_record(QSqlRecord &r) {
    r.setValue("name", this->name);
    r.setValue("salary", this->salary);
}

void Employee::remove() {
    this->_remove<Employee>();
}

QString Employee::getName() const {
    return this->name;
}

void Employee::setName(QString name) {
    this->name = name;
}

int Employee::getSalary() const {
    return this->salary;
}

void Employee::setSalary(int salary) {
    this->salary = salary;
}

std::vector<Project*> Employee::getProjects() {
    return this->projects.getValues();
}
