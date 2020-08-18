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
  connectClientToServer();
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
  QObject::connect(_widgetLanding, &Landing::alert, this, &GuiManager::alert);
  QObject::connect(_widgetLogin, &Login::alert, this, &GuiManager::alert);
  QObject::connect(_widgetDocsBrowser, &DocsBrowser::alert, this, &GuiManager::alert);
  QObject::connect(_widgetEdit, &Edit::alert, this, &GuiManager::alert);
  QObject::connect(_widgetRegistration, &Registration::alert, this, &GuiManager::alert);
  QObject::connect(_widgetTextEditor, &TextEditor::alert, this, &GuiManager::alert);

  // user/file updates
  QObject::connect(_user.get(), &User::iconChanged, _widgetDocsBrowser, &DocsBrowser::setIcon);  QObject::connect(_user.get(), &User::iconChanged, _widgetEdit, &Edit::setIcon);

  //TODO others e.g. icon in texteditor

  //Login
  QObject::connect(_widgetLogin, &Login::login, this, &GuiManager::loginLogin);
  QObject::connect(_widgetLogin, &Login::signup, this, &GuiManager::loginSignup);

  //Registration
  QObject::connect(_widgetRegistration, &Registration::signup, this, &GuiManager::registrationSignup);
  QObject::connect(_widgetRegistration, &Registration::cancel, this, &GuiManager::registrationCancel);

  // Edit
  QObject::connect(_widgetEdit, &Edit::save, this, &GuiManager::editSave);
  QObject::connect(_widgetEdit, &Edit::cancel, this, &GuiManager::editCancel);

  //DocsBrowser
  QObject::connect(_widgetDocsBrowser, &DocsBrowser::logout, this, &GuiManager::docsBrowserLogout);
  QObject::connect(_widgetDocsBrowser, &DocsBrowser::editAccount, this, &GuiManager::docsBrowserEditAccount);
  QObject::connect(_widgetDocsBrowser, &DocsBrowser::newFile, this, &GuiManager::docsBrowserNewFile);


  //TextEditor
  //TODO
}

void GuiManager::connectClientToServer() {
  //Comunicazioni Client -> Server
  QObject::connect(this, &GuiManager::loginQuery, _manager.get(), &MessageManager::loginQuery);
  QObject::connect(this, &GuiManager::logoutQuery, _manager.get(), &MessageManager::logoutQuery);
  QObject::connect(this, &GuiManager::newUserQuery, _manager.get(), &MessageManager::newUserQuery);
  QObject::connect(this, &GuiManager::editUserQuery, _manager.get(), &MessageManager::editUserQuery);
  QObject::connect(this, &GuiManager::deleteUserQuery, _manager.get(), &MessageManager::deleteUserQuery);

  QObject::connect(this, &GuiManager::newFileQuery, _manager.get(), &MessageManager::newFileQuery);
  QObject::connect(this, &GuiManager::getFileQuery, _manager.get(), &MessageManager::getFileQuery);
  QObject::connect(this, &GuiManager::closeFileQuery, _manager.get(), &MessageManager::closeFileQuery);
  QObject::connect(this, &GuiManager::editFileQuery, _manager.get(), &MessageManager::editFileQuery);
  QObject::connect(this, &GuiManager::deleteFileQuery, _manager.get(), &MessageManager::deleteFileQuery);
  QObject::connect(this, &GuiManager::getLinkQuery, _manager.get(), &MessageManager::getLinkQuery);
  QObject::connect(this, &GuiManager::activateLinkQuery, _manager.get(), &MessageManager::activateLinkQuery);

  QObject::connect(this, &GuiManager::localInsertQuery, _manager.get(), &MessageManager::localInsertQuery);
  QObject::connect(this, &GuiManager::localDeleteQuery, _manager.get(), &MessageManager::localDeleteQuery);
  QObject::connect(this, &GuiManager::localUpdateQuery, _manager.get(), &MessageManager::localUpdateQuery);
  QObject::connect(this, &GuiManager::localMoveQuery, _manager.get(), &MessageManager::localMoveQuery);

  QObject::connect(this, &GuiManager::newDirQuery, _manager.get(), &MessageManager::newDirQuery);
  QObject::connect(this, &GuiManager::editDirQuery, _manager.get(), &MessageManager::editDirQuery);
  QObject::connect(this, &GuiManager::deleteDirQuery, _manager.get(), &MessageManager::deleteDirQuery);
  QObject::connect(this, &GuiManager::getDirQuery, _manager.get(), &MessageManager::getDirQuery);
  QObject::connect(this, &GuiManager::moveFileQuery, _manager.get(), &MessageManager::moveFileQuery);

  QObject::connect(this, &GuiManager::commentLocalInsertQuery, _manager.get(), &MessageManager::commentLocalInsertQuery);
  QObject::connect(this, &GuiManager::commentLocalUpdateQuery, _manager.get(), &MessageManager::commentLocalUpdateQuery);
  QObject::connect(this, &GuiManager::commentLocalDeleteQuery, _manager.get(), &MessageManager::commentLocalDeleteQuery);
}

void GuiManager::connectServerToClient() {
  //Comunicazioni Server -> Client
  QObject::connect(_manager.get(), &MessageManager::errorResponse, this, &GuiManager::serverErrorResponse);
  QObject::connect(_manager.get(), &MessageManager::loginResponse, this, &GuiManager::serverLoginResponse);
  QObject::connect(_manager.get(), &MessageManager::newUserResponse, this, &GuiManager::serverNewUserResponse);
  QObject::connect(_manager.get(), &MessageManager::editUserResponse, this, &GuiManager::serverEditUserResponse);
  QObject::connect(_manager.get(), &MessageManager::newFileResponse, this, &GuiManager::serverNewFileResponse);
}

void GuiManager::connected() {
  info("Connesso al server");
  showWindow(_widgetLogin, true);
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

void GuiManager::alert(Alert type, const QString &what) {
  //TODO show alert dialog or something
  debug("ALERT: " + QString::number(static_cast<int>(type)) + " " + what);
}

void GuiManager::showWindow(MainWindow *window, bool clear) {
  if(clear)
    window->clear();

  _stackedWidget->setCurrentWidget(window);
}

void GuiManager::freezeWindow() {
  auto widget = static_cast<MainWindow *>(_stackedWidget->currentWidget());
  widget->freeze();
}

void GuiManager::unfreezeWindow() {
  auto widget = static_cast<MainWindow *>(_stackedWidget->currentWidget());
  widget->unfreeze();
}

/* ### LOGIN ### */

void GuiManager::loginLogin(const QString &username, const QString &password) {
  freezeWindow();
  emit loginQuery(username, password);
}

void GuiManager::loginSignup() {
  showWindow(_widgetRegistration, true);
}

/* ### REGISTRATION ### */

void GuiManager::registrationCancel() {
  showWindow(_widgetLogin, true);
}

void GuiManager::registrationSignup(const QString &username, const QString &password, const QString &pswRepeat, const std::optional<QString> &icon) {
  freezeWindow();
  emit newUserQuery(username, password, pswRepeat, icon);
}

/* ### EDIT ### */
void GuiManager::editSave(const QString &token, const std::optional<QString> &oldPassword,
  const std::optional<QString> &password, const std::optional<QString> &pswRepeat, const std::optional<QString> &icon) {
    freezeWindow();
    emit editUserQuery(token, oldPassword, password, pswRepeat, icon);
}

void GuiManager::editCancel() {
  showWindow(_widgetDocsBrowser);
}

/* ### DOCSBROWSER ### */

void GuiManager::docsBrowserLogout(const QString &token) {
  emit logoutQuery(token);
  _user->logout();
  showWindow(_widgetLogin, true);
}

void GuiManager::docsBrowserEditAccount() {
  showWindow(_widgetEdit, true);
}

void GuiManager::docsBrowserNewFile(const QString &token, const QString &name, const std::optional<int> &dirId) {
  freezeWindow();
  emit newFileQuery(token, name, dirId);
}

/* ### MESSAGES FROM SERVER ### */

void GuiManager::serverErrorResponse(const QString &reason) {
  unfreezeWindow();
  alert(Alert::ERROR, reason);
}

void GuiManager::serverLoginResponse(const QString &token, int userId, const std::optional<QString> &icon) {
  debug("GuiManager::serverLoginResponse");
  _user->login(token, userId, icon);
  unfreezeWindow();
  showWindow(_widgetEdit, true); // to fix small icon bug in Edit when launched first time
  showWindow(_widgetDocsBrowser, true);
}

void GuiManager::serverNewUserResponse(const QString &token, int userId) {
  debug("GuiManager::serverNewUserResponse");
  _user->login(token, userId, std::nullopt);
  unfreezeWindow();
  showWindow(_widgetEdit, true); // to fix small icon bug in Edit when launched first time
  showWindow(_widgetDocsBrowser, true);
}

void GuiManager::serverEditUserResponse() {
  debug("GuiManager::serverEditUserResponse");
  _user->tempToPermanentIcon();
  unfreezeWindow();
  showWindow(_widgetDocsBrowser); //TODO ?
}

void GuiManager::serverNewFileResponse(int fileId) {
  debug("GuiManager::serverNewFileResponse");
  _user->openFile(fileId);
  unfreezeWindow();
  showWindow(_widgetTextEditor, true);
}
