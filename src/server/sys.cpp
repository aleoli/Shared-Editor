#include "sys.h"

#include <iostream>

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QDir>
#include <QTime>

#include "def.h"
#include "persistence_global.h"
#include "exceptions.h"
#include "sets.h"

SysConf parse_arguments(QCommandLineParser &parser, const QCoreApplication &app);

SysConf initiate_system(const QCoreApplication &app) {
  QCoreApplication::setApplicationVersion("1.0");
  QCoreApplication::setApplicationName("Shared Editor - Server");

  QCommandLineParser parser;
  parser.setApplicationDescription("Server for Shared-Editor application");

  auto conf = parse_arguments(parser, app);

  // creo la cartella in cui salverà tutti i suoi file, se non esiste
  QDir dir(QDir::homePath()+"/.shared_editor");
  bool dir_exists = dir.exists();
  if(!dir_exists) {
    dir.mkdir(QDir::homePath()+"/.shared_editor");
  }

  // creo la cartella data, se non esiste
  QDir dirData(QDir::homePath()+"/.shared_editor/data");
  dir_exists = dirData.exists();
  if(!dir_exists) {
    dirData.mkdir(QDir::homePath()+"/.shared_editor/data");
  }

  // lo devo controllare prima della creazione del DB
  bool file_exists = dir.exists("shared-editor.db");

  // mi assicuro che l'istanza del DB sia richimata almeno una volta prima della prima query
  auto db = DB::get(conf.mem_only);
  (void) db;

  if(!dir_exists || !file_exists || conf.mem_only) {
    DB::create_db_if_not_exists();
  }

  // seed for random generator
  qsrand(static_cast<quint64>(QTime::currentTime().msecsSinceStartOfDay()));

  return conf;
}

SysConf parse_arguments(QCommandLineParser &parser, const QCoreApplication &app) {
  const QCommandLineOption helpOption = parser.addHelpOption();
  const QCommandLineOption versionOption = parser.addVersionOption();
  parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
  const QCommandLineOption portOption({"p", "port"}, "Port to bind.", "Port", QString::number(DEF_PORT));
  const QCommandLineOption moOption({"m", "mem-only"}, "Create Mem-Only DB (only for testing and debugging).");
  const QCommandLineOption logOption({"l", "log"}, "Log Level [NONE, error, warn, info, debug].", "LogLevel", "none");

  parser.addOptions({portOption, moOption, logOption});

  parser.process(app);

  SysConf conf;
  bool ok;
  conf.port = parser.value(portOption).toInt(&ok);
  if(!ok) {
    throw se_exceptions::ArgNotValidException{"Not valid port '"+parser.value(portOption)+"'"};
  }
  conf.mem_only = parser.isSet(moOption);

  auto sets = Sets::get(parser.value(logOption).toLower());

  return conf;
}
