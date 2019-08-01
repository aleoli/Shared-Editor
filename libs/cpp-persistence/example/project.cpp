#include "project.h"

#include "employee.h"

const QString Project::table_name = "project";

Project::Project(): Persistent(), name("") {
}

Project::Project(const Project &e): Persistent(e) {
    this->name = e.name;
    this->employee = e.employee;
}

Project::Project(Project &&e): Persistent(e) {
    this->name = e.name;
    this->employee = e.employee;
}

Project::Project(QSqlRecord r): Persistent(r) {
    this->name = r.value("name").toString();
    this->employee = Lazy<Employee>{r.value("employee").toInt()};
}

Project::~Project() {}

Project& Project::operator=(const Project& e) {
    if(this == &e) {
        return *this;
    }
    Persistent::operator=(e);
    this->name = e.name;
    this->employee = e.employee;
    return *this;
}

void Project::save() {
    this->_save<Project>();
}

void Project::save_record(QSqlRecord &r) {
    r.setValue("name", this->name);
    r.setValue("employee", this->employee.getId());
}

void Project::remove() {
    this->_remove<Project>();
}

QString Project::getName() const {
    return this->name;
}

int Project::getEmployeeId() const {
    return this->employee.getId();
}

Employee Project::getEmployee() {
    return this->employee.getValue();
}
