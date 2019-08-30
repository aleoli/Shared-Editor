#include <iostream>

#include <QDir>

#include "persistence_global.h"

void initiate_system() {
  // creo la cartella in cui salverà tutti i suoi file, se non esiste
  QDir dir(QDir::homePath()+"/.shared_editor");
  if(!dir.exists()) {
    dir.mkdir(QDir::homePath()+"/.shared_editor");
  }

  // mi assicuro che l'istanza del DB sia richimata almeno una volta prima della prima query
  DB *db = DB::get();
  (void) db;
}
