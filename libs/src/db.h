#ifndef DB_H
#define DB_H

#include <vector>
#include <iostream>

#include <QSqlTableModel>
#include <QSqlRecord>

#include "persistence.h"

#include "sqlexception.h"

class persistence::DB {
public:
    static DB* get() {
        static DB* db = new DB{};
        return db;
    }

    template<typename T> static std::vector<T> getAll() {
        std::vector<T> v;
        QSqlTableModel model;
        model.setTable(T::table_name);
        model.select();
        for(int i=0; i<model.rowCount(); ++i) {
            v.push_back(T{model.record(i)});
        }
        return v;
    }

    template<typename T> static T getOne(int id) {
        QSqlTableModel model;
        model.setTable(T::table_name);
        model.setFilter("id = "+QString::number(id));
        model.select();
        if(model.rowCount() > 0) {
            return T{model.record(0)};
        } else {
            throw SQLNoElementSelectException{"No element found in table '"+T::table_name+"' for id "+QString::number(id)};
        }
    }

    template<typename T> static std::vector<T> get(QString filter) {
        std::vector<T> v;
        QSqlTableModel model;
        model.setTable(T::table_name);
        model.setFilter(filter);
        model.select();
        for(int i=0; i<model.rowCount(); ++i) {
            v.push_back(T{model.record(i)});
        }
        return v;
    }

private:
    explicit DB() {
        QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
        db.setHostName("192.168.2.10");
        db.setDatabaseName("cpp_persistence");
        db.setUserName("cpp_persistence");
        db.setPassword("cpp_persistence");
        bool ok = db.open();

        if(!ok) {
            std::cout << "ERRORE: connessione al DB fallita" << std::endl;
            exit(1);
        }

        std::cout << "DB connesso" << std::endl;
    }
};

#endif // DB_H
