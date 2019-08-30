#include <iostream>
#include <vector>

#include <QDir>

#include "persistence_global.h"
#include "sys.h"

int main(int argc, char *argv[]) {
  initiate_system();

  DB::create_db_if_not_exists("src/files/db.sql");

  return 0;
}
