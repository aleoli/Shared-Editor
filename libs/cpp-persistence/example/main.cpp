#include <QCoreApplication>

#include "persistence_global.h"
#include "example.h"

#include <vector>
#include <iostream>

#define MODIFY 1
#define INSERT 1
#define REMOVE 1

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    DB *db = DB::get();
    (void) db;

    std::vector<Employee> emp_list(DB::getAll<Employee>());

    for(auto it=emp_list.begin(); it!=emp_list.end(); ++it) {
        std::cout << it->getName().toStdString() << it->getSalary() << std::endl;
    }

    auto e = DB::getOne<Employee>(1);
    std::cout << "ID 1" << e.getName().toStdString() << e.getSalary() << std::endl;

    auto high_sal = DB::get<Employee>("salary > 5000");
    for(auto it=high_sal.begin(); it!=high_sal.end(); ++it) {
        std::cout << "High Salary: " << it->getName().toStdString() << it->getSalary() << std::endl;
#if MODIFY
        it->setSalary(it->getSalary()*1.1);
        it->save();
#endif
    }

#if INSERT
    Employee e_nuovo{};
    e_nuovo.setName("terzo");
    e_nuovo.setSalary(6000);
    e_nuovo.save();
#endif

#if REMOVE
    Employee e_rem = DB::getOne<Employee>(3);
    e_rem.remove();
#endif

    std::vector<Project> pr_list(DB::getAll<Project>());
    for(auto it=pr_list.begin(); it!=pr_list.end(); ++it) {
        std::cout << it->getName().toStdString() << it->getEmployeeId() << it->getEmployee().getName().toStdString() << std::endl;
    }


    std::cout << "Progetti di "+e.getName().toStdString() << std::endl;
    std::vector<Project*> e_proj = e.getProjects();
    for(Project* it: e_proj) {
        std::cout << "\t" << it->getName().toStdString() << std::endl;
    }

    return a.exec();
}
