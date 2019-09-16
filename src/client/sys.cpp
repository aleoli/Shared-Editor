#include "sys.h"

#include <iostream>

#include <QApplication>
#include <QCommandLineParser>

#include "def.h"
#include "exceptions.h"

SysConf parse_arguments(QCommandLineParser &parser, const QApplication &app);

SysConf initiate_system(const QApplication &app) {
  QApplication::setApplicationVersion("1.0");
  QApplication::setApplicationName("Shared Editor");

  QCommandLineParser parser;
  parser.setApplicationDescription("Client for Shared-Editor application");

  return parse_arguments(parser, app);
}

SysConf parse_arguments(QCommandLineParser &parser, const QApplication &app) {
  const QCommandLineOption helpOption = parser.addHelpOption();
  const QCommandLineOption versionOption = parser.addVersionOption();
  parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
  QCommandLineOption hOption({"s", "server"}, "Server to connect.", "Server", DEF_HOST);
  QCommandLineOption portOption({"p", "port"}, "Port to connect.", "Port", QString::number(DEF_PORT));

  parser.addOptions({hOption, portOption});

  parser.process(app);

  SysConf conf;
  conf.host = parser.value(hOption);
  bool ok;
  conf.port = parser.value(portOption).toInt(&ok);
  if(!ok) {
    throw se_exceptions::ArgNotValidException{"Not valid port '"+parser.value(portOption)+"'"};
  }

  return conf;
}
