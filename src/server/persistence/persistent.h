#ifndef PERSISTENT_H
#define PERSISTENT_H

#include "persistence.h"

#include "exceptions.h"

using namespace se_exceptions;

#include <QSqlTableModel>
#include <QSqlRecord>

class persistence::Persistent {
public:
    explicit Persistent(): id(0) {}

    Persistent(const Persistent &e) {
        this->id = e.id;
    }

    Persistent(Persistent &&e) {
        this->id = e.id;
    }

    Persistent(QSqlRecord r) {
        this->id = r.value("id").toInt();
    }

    virtual ~Persistent() {}

    virtual Persistent& operator=(const Persistent& e) {
        if(this == &e) {
            return *this;
        }
        this->id = e.id;
        return *this;
    }

    virtual void save() = 0;
    virtual void save_record(QSqlRecord &) = 0;
    virtual void remove() = 0;

    const static QString table_name;

    int getId() const {
        return this->id;
    }

protected:
    template<typename T>
    void _save() {
        QSqlTableModel model;
        model.database().transaction();
        model.setTable(T::table_name);
        if(this->id > 0) {
            model.setFilter("id = "+QString::number(this->id));
            model.select();
            if(model.rowCount() > 0) {
                auto record = model.record(0);
                this->save_record(record);
                model.setRecord(0, record);
            } else {
                model.database().rollback();
                throw SQLNoElementUpdateException{"No element found in table '"+T::table_name+"' for update of id "+QString::number(this->id)};
            }
        } else {
            auto record = model.record();
            record.remove(record.indexOf("id"));        // rimuovo la colonna id perchè è auto-increment
            this->save_record(record);
            if(!model.insertRecord(-1, record)) {
                model.database().rollback();
                throw SQLInsertException{"Failed insertion in table "+T::table_name};
            }
        }
        model.submitAll();
        model.database().commit();
    }

    template<typename T>
    void _remove() {
        if(this->id <= 0) {
            return;
        }
        QSqlTableModel model;
        model.database().transaction();
        model.setTable(T::table_name);
        model.setFilter("id = "+QString::number(this->id));
        model.select();

        if(model.rowCount() > 0) {
            model.removeRow(0);
            model.submitAll();
        } else {
            throw SQLNoElementDeleteException{"No element found in table '"+T::table_name+"' for delete of id "+QString::number(this->id)};
        }

        model.submitAll();
        model.database().commit();
    }

    int id;
};

#endif // PERSISTENT_H
