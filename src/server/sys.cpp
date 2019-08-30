#include <iostream>

#include <QDir>

#include "persistence_global.h"

void initiate_system() {
  // creo la cartella in cui salverà tutti i suoi file, se non esiste
  QDir dir(QDir::homePath()+"/.shared_editor");
  bool dir_exists = dir.exists();
  if(!dir_exists) {
    dir.mkdir(QDir::homePath()+"/.shared_editor");
  }

  // lo devo controllare prima della creazione del DB
  bool file_exists = dir.exists("shared-editor.db");

  // mi assicuro che l'istanza del DB sia richimata almeno una volta prima della prima query
  DB *db = DB::get();
  (void) db;

  if(!dir_exists || !file_exists) {
    DB::create_db_if_not_exists();
  }
}
