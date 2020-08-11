#ifndef DB_H
#define DB_H

#include <vector>
#include <iostream>
#include <memory>

#include <QSqlTableModel>
#include <QSqlRecord>
#include <QSqlError>
#include <QSqlQuery>

#include <QDir>
#include <QFile>
#include <QTextStream>

#include "persistence.h"

#include "utils.h"
#include "exceptions.h"

using namespace se_exceptions;

class persistence::DB {
public:
  DB(const DB&) = delete;
  DB(DB&&) = delete;
  DB& operator=(const DB&) = delete;
  DB& operator=(DB&&) = delete;

  static std::shared_ptr<DB> get(bool mem_only = false) {
    if(DB::instance == nullptr) {
      DB::instance.reset(new DB{mem_only});
    }
    return DB::instance;
  }

  static void create_db_if_not_exists(const QString& path = "") {
    QSqlQuery query;
    QStringList strs;
    if(path != "") {
      QFile f(path);
      if(!f.open(QIODevice::ReadOnly)) {
        std::cerr << "ERRORE: non trovo il file per la creazione del database" << std::endl;
        exit(1);
      }
      strs = QTextStream(&f).readAll().split(";");
    } else {
      strs = DB::get()->create_db;
    }

    for(QString& str: strs) {
      if(str.trimmed().isEmpty()) {
        continue;
      }
      if(!query.exec(str)) {
        std::cerr << "ERRORE: " << query.lastError().text().toStdString() << std::endl;
        exit(1);
      }
      query.finish();
    }
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

  template<typename T> static std::vector<T> get(const QString& filter) {
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
    static std::shared_ptr<DB> instance;

    explicit DB(bool mem_only = false) {
      if(!QSqlDatabase::isDriverAvailable("QSQLITE")) {
        std::cerr << "ERRORE: No Driver" << std::endl;
        exit(1);    // se non ha i driver del DB posso anche interrompere il programma, tanto non può funzionare
      }
      db = QSqlDatabase::addDatabase("QSQLITE");
      if(mem_only) {
        debug("mem-only database");
        db.setDatabaseName(":memory:");
      } else {
        db.setDatabaseName(QDir::homePath()+"/.shared_editor/shared-editor.db");
      }

      if(!db.open()) {
          std::cerr << "ERRORE: connessione al DB fallita" << std::endl << db.lastError().text().toStdString() << std::endl;
          exit(1);  // idem come sopra
      }

      debug("DB connesso");
    }

    QSqlDatabase db;
    QStringList create_db = {
      "CREATE TABLE IF NOT EXISTS user (id INTEGER PRIMARY KEY AUTOINCREMENT, username VARCHAR(255) UNIQUE, password VARCHAR(255));",
      "CREATE TABLE IF NOT EXISTS session (id INTEGER PRIMARY KEY AUTOINCREMENT, token VARCHAR(255) UNIQUE, active INT, user_id INT, FOREIGN KEY(user_id) REFERENCES user(id));",
      "CREATE TABLE IF NOT EXISTS fs_element (id INTEGER PRIMARY KEY AUTOINCREMENT, path VARCHAR(255), name VARCHAR(255), type INT, parent_id INT, owner_id INT, creator_id INT, linked_from INT, char_id INT DEFAULT(0), FOREIGN KEY(owner_id) REFERENCES user(id), FOREIGN KEY(creator_id) REFERENCES user(id), FOREIGN KEY(parent_id) REFERENCES fs_element(id), FOREIGN KEY(linked_from) REFERENCES fs_element(id))",
      "CREATE TABLE IF NOT EXISTS shared_link (id INTEGER PRIMARY KEY AUTOINCREMENT, token VARCHAR(128) UNIQUE, element_id INT, FOREIGN KEY(element_id) REFERENCES fs_element(id))"
    };
};

#endif // DB_H
