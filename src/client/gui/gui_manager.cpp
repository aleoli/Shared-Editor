#include "gui_manager.h"

#include <QThread>
#include <QString>

#include "exceptions.h"
#include "utils.h"

#include "login.h"
#include "docsbrowser.h"
#include "edit.h"
#include "landing.h"
#include "registration.h"
#include "texteditor.h"

using namespace se_exceptions;

std::shared_ptr<GuiManager> GuiManager::instance = nullptr;

GuiManager::GuiManager(const SysConf &conf, QObject *parent): QObject(parent) {
  initThreads(conf);

  _stackedWidget = new MyStackedWidget();
  _user = User::get();

  //TODO crea finestre e mettile nello stacked
  //TODO collega tutti i segnali
}

GuiManager::~GuiManager() {
  delete _stackedWidget;

  _serverThread->quit();
  _managerThread->quit();

  _serverThread->wait();
  _managerThread->wait();
}

std::shared_ptr<GuiManager> GuiManager::get(std::optional<SysConf> conf) {
  if(instance == nullptr) {
    if(conf)
      instance.reset(new GuiManager(*conf));
    else
      throw GuiException{"No configuration provided!"};
  }
  return instance;
}

void GuiManager::initThreads(const SysConf &conf) {
  // inizializzazione threads
  _manager = MessageManager::get();
  _server = Server::get(conf.host, conf.port);

  _serverThread = new QThread{this};
  _server->moveToThread(_serverThread);

  _managerThread = new QThread{this};
  _manager->moveToThread(_managerThread);

  QObject::connect(_serverThread, SIGNAL(started()), _server.get(), SLOT(connect()));
  QObject::connect(_server.get(), SIGNAL(connected()), this, SLOT(connected()));
  QObject::connect(_server.get(), SIGNAL(disconnected()), this, SLOT(connectionLost()));
  QObject::connect(_server.get(), SIGNAL(dataReady(QByteArray)), _manager.get(), SLOT(process_data(QByteArray)));
  QObject::connect(_server.get(), SIGNAL(connection_error()), this, SLOT(connectionLost()));
  QObject::connect(_manager.get(), SIGNAL(connection_error()), _server.get(), SLOT(disconnect()));
  QObject::connect(_manager.get(), SIGNAL(send_data(QByteArray)), _server.get(), SLOT(write(QByteArray)));
}

void GuiManager::connected() {
  info("Connesso al server");

  // close landing, show login
}

void GuiManager::connectionLost() {
  warn("La connessione è stata persa");

  //TODO display messaggio su schermo (?)

  //TODO provare a recuperare la connessione invece di chiudere (?)
  emit quit();
}

void GuiManager::run() {
  info("GuiManager running");
  _serverThread->start();
  _managerThread->start();

  //TODO show landing
}
