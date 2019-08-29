#ifndef DB_H
#define DB_H

#include <vector>
#include <iostream>

#include <QSqlTableModel>
#include <QSqlRecord>
#include <QSqlError>

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
        if(!QSqlDatabase::isDriverAvailable("QSQLITE")) {
          std::cerr << "ERRORE: No Driver" << std::endl;
          exit(1);    // se non ha i driver del DB posso anche interrompere il programma, tanto non può funzionare
        }
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
#if IN_MEMORY
        db.setDatabaseName(":memory:");
#else
        db.setDatabaseName("shared-editor.db");
#endif

        if(!db.open()) {
            std::cerr << "ERRORE: connessione al DB fallita" << std::endl << db.lastError().text().toStdString() << std::endl;
            exit(1);  // idem come sopra
        }

        std::cout << "DB connesso" << std::endl;
    }
};

#endif // DB_H
