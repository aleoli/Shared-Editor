#pragma once

#include "persistence.h"

#include <utility>
#include <vector>

#include "db.h"
#include "exceptions.h"

using namespace se_exceptions;

using persistence::DB;

template<typename T>
class persistence::LazyList {
public:
    LazyList(): has_values(false) {}
    explicit LazyList(std::vector<int> ids): has_values(false), has_query(false), ids(std::move(ids)) {}
    LazyList(QString table_name, QString filter): has_values(false), has_query(true), table_name(std::move(table_name)), filter(std::move(filter)) {}

    LazyList(const LazyList &e) {
        if(this != &e) {
            for(int i=0; i<this->values.size(); i++) {
                delete this->values[i];
                this->values[i] = nullptr;
            }
            this->values.clear();
            this->ids.clear();
            this->ids = e.ids;
            for(T* it: e.values) {
                this->values.push_back(new T{*it});
            }
            this->has_values = e.has_values;
            this->has_query = e.has_query;
            this->table_name = e.table_name;
            this->filter = e.filter;
        }
    }

    LazyList(LazyList &&e) noexcept {
        if(this != &e) {
            for(int i=0; i<this->values.size(); i++) {
                delete this->values[i];
                this->values[i] = nullptr;
            }
            this->values.clear();
            this->ids.clear();
            this->ids = e.ids;
            for(T* it: e.values) {
                this->values.push_back(it);
            }
            e.ids.clear();
            e.values.clear();
            this->has_values = e.has_values;
            e.has_values = false;
            this->has_query = e.has_query;
            this->table_name = e.table_name;
            this->filter = e.filter;
        }
    }

    ~LazyList() {
        for(int i=0; i<this->values.size(); i++) {
            delete this->values[i];
            this->values[i] = nullptr;
        }
    }

    LazyList& operator =(const LazyList &e) {
        if(this != &e) {
            for(int i=0; i<this->values.size(); i++) {
                delete this->values[i];
                this->values[i] = nullptr;
            }
            this->values.clear();
            this->ids.clear();
            this->ids = e.ids;
            for(T* it: e.values) {
                this->values.push_back(new T{*it});
            }
            this->has_values = e.has_values;
            this->has_query = e.has_query;
            this->table_name = e.table_name;
            this->filter = e.filter;
        }
        return *this;
    }

    LazyList& operator =(LazyList &&e) noexcept {
        if(this != &e) {
            for(int i=0; i<this->values.size(); i++) {
                delete this->values[i];
                this->values[i] = nullptr;
            }
            this->values.clear();
            this->ids.clear();
            this->ids = e.ids;
            for(T* it: e.values) {
                this->values.push_back(it);
            }
            e.ids.clear();
            e.values.clear();
            this->has_values = e.has_values;
            e.has_values = false;
            this->has_query = e.has_query;
            this->table_name = e.table_name;
            this->filter = e.filter;
        }
        return *this;
    }

    std::vector<T*> operator ->() {
        if(!this->has_values) {
            this->values.clear();
            if(this->has_query) {
                this->load_from_query();
            } else {
                for(auto it: this->ids) {
                    this->values.push_back(new T{DB::getOne<T>(it)});
                }
            }
            this->has_values = true;
        }
        return this->values;
    }

    T& operator [](int id) {
        this->getValues();
        for(auto it: this->values) {
            if(it->getId() == id) {
                return *it;
            }
        }
        throw SQLNoElementSelectException{"No element found in list for id "+QString::number(id)};
    }

    std::vector<T*> getValues() {
        return this->operator->();
    }

    std::vector<int> getIds() {
        if(this->has_query) {
            this->load_from_query();
            this->has_query = false;
        }
        return this->ids;
    }

		void clear() {
				for(int i=0; i<this->values.size(); i++) {
						delete this->values[i];
				}
				this->ids.clear();
				this->values.clear();
				this->has_values = false;
		}

    void addValue(T* v) {
        this->ids.push_back(v->getId());
        this->values.push_back(v);
    }

private:
    void load_from_query() {
        QSqlTableModel model;
        model.setTable(this->table_name);
        model.setFilter(this->filter);
        model.select();
        this->ids.clear();
        this->values.clear();
        for(int i=0; i<model.rowCount(); ++i) {
            this->values.push_back(new T{model.record(i)});
            this->ids.push_back(model.record(i).value("id").toInt());
        }
        //this->has_query = false;
        this->has_values = true;
    }

    std::vector<int> ids;
    std::vector<T*> values;
    bool has_values{};
    bool has_query{};
    QString table_name;
    QString filter;
};
