#include "gui_manager.h"

#include <QThread>
#include <QString>

#include <optional>
#include "exceptions.h"

using namespace se_exceptions;

std::shared_ptr<GuiManager> GuiManager::instance = nullptr;

GuiManager::GuiManager(const SysConf &conf, QObject *parent): QObject(parent) {
  initThreads(conf);

  _stackedWidget = new MyStackedWidget();
  _user = User::get();

  _widgetLanding = new Landing;
  _widgetLogin = new Login;
  _widgetDocsBrowser = new DocsBrowser;
  _widgetEdit = new Edit;
  _widgetRegistration = new Registration;
  _widgetTextEditor = new TextEditor;

  // lo stacked prende l'ownership di queste finestre
  _stackedWidget->addWidget(_widgetLanding);
  _stackedWidget->addWidget(_widgetLogin);
  _stackedWidget->addWidget(_widgetDocsBrowser);
  _stackedWidget->addWidget(_widgetEdit);
  _stackedWidget->addWidget(_widgetRegistration);
  _stackedWidget->addWidget(_widgetTextEditor);

  _stackedWidget->setCurrentWidget(_widgetLanding);

  connectWidgets();
  connectServerToClient();
}

GuiManager::~GuiManager() {
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

void GuiManager::connectWidgets() {
  QObject::connect(_stackedWidget, SIGNAL(close()), this, SLOT(closeStacked()));

  // alerts
  QObject::connect(_widgetLanding, SIGNAL(alert(Alert, QString)), this, SLOT(alert(Alert, QString)));
  QObject::connect(_widgetLogin, SIGNAL(alert(Alert, QString)), this, SLOT(alert(Alert, QString)));
  QObject::connect(_widgetDocsBrowser, SIGNAL(alert(Alert, QString)), this, SLOT(alert(Alert, QString)));
  QObject::connect(_widgetEdit, SIGNAL(alert(Alert, QString)), this, SLOT(alert(Alert, QString)));
  QObject::connect(_widgetRegistration, SIGNAL(alert(Alert, QString)), this, SLOT(alert(Alert, QString)));
  QObject::connect(_widgetTextEditor, SIGNAL(alert(Alert, QString)), this, SLOT(alert(Alert, QString)));

  //Login
  QObject::connect(_widgetLogin, &Login::login, _manager.get(), &MessageManager::loginQuery);
  QObject::connect(_widgetLogin, &Login::signup, this, &GuiManager::showRegistration);

  //Registration
  QObject::connect(_widgetRegistration, &Registration::signup, _manager.get(), &MessageManager::newUserQuery);
  QObject::connect(_widgetRegistration, &Registration::cancel, this, &GuiManager::showLogin);

  // Edit
  //TODO

  //DocsBrowser
  //TODO

  //TextEditor
  //TODO
}

void GuiManager::connectServerToClient() {
  //Comunicazioni Server -> Client
  QObject::connect(_manager.get(), &MessageManager::errorResponse, this, &GuiManager::serverErrorResponse);
  QObject::connect(_manager.get(), &MessageManager::loginResponse, this, &GuiManager::serverLoginResponse);
  QObject::connect(_manager.get(), &MessageManager::newUserResponse, this, &GuiManager::serverNewUserResponse);
}

void GuiManager::connected() {
  info("Connesso al server");
  showLogin();
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
  _stackedWidget->show();
}

void GuiManager::closeStacked() {
  debug("Pressed close button");

  //TODO logica per vedere se veramente si deve chiudere l'app oppure no?
  emit quit();
}

void GuiManager::alert(Alert type, QString what) {
  //TODO show alert dialog or something
  debug("ALERT: " + QString::number(static_cast<int>(type)) + " " + what);
}

//TODO servono dei check per queste funzioni di show?
void GuiManager::showRegistration() {
  _stackedWidget->setCurrentWidget(_widgetRegistration);
}

void GuiManager::showDocsBrowser() {
  _stackedWidget->setCurrentWidget(_widgetDocsBrowser);
}

void GuiManager::showLogin() {
  _stackedWidget->setCurrentWidget(_widgetLogin);
}

void GuiManager::showEdit() {
  _stackedWidget->setCurrentWidget(_widgetEdit);
}

void GuiManager::showTextEditor() {
  _stackedWidget->setCurrentWidget(_widgetTextEditor);
}

/* ### messages from server ### */

void GuiManager::serverErrorResponse(QString reason) {
  //TODO basta cosi?
  alert(Alert::ERROR, reason);
}

void GuiManager::serverLoginResponse(QString token, int userId, std::optional<QString> icon) {
  debug("GuiManager::serverLoginResponse");
  _user->login(token, userId, icon);
  showDocsBrowser();
}

void GuiManager::serverNewUserResponse(QString token, int userId) {
  debug("GuiManager::serverNewUserResponse");
  //TODO
  _user->login(token, userId, std::nullopt);
  showDocsBrowser();
}
