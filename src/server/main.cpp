#include <iostream>
#include <vector>

#include "persistence_global.h"

int main(int argc, char *argv[]) {
  DB *db = DB::get();
  (void) db;

  std::cout << "Ciao mondo!" << std::endl;

  DB::create_db_if_not_exists("../files/db.sql");

  return 0;
}
