#include <iostream>
#include <vector>

#include "persistence_global.h"

#include <QSqlQuery>

int main(int argc, char *argv[]) {
  DB *db = DB::get();
  (void) db;

  std::cout << "Ciao mondo!" << std::endl;

  QSqlQuery query;
  query.exec("CREATE TABLE people (id INTEGER PRIMARY KEY, name TEXT)");

  return 0;
}
