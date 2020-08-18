#include "gui_manager.h"

#include <QThread>
#include <QString>
#include <QRect>
#include <QApplication>
#include <QDesktopWidget>
#include <QTimer>

#include <optional>
#include "exceptions.h"
#include "info.h"
#include "confirm.h"

using namespace se_exceptions;

std::shared_ptr<GuiManager> GuiManager::instance = nullptr;

GuiManager::GuiManager(const SysConf &conf, QObject *parent)
  : QObject(parent), _connected(false) {
  initThreads(conf);

  _stackedWidget = new StackedWidget();
  _user = User::get();

  _widgetLogin = new Login;
  _widgetDocsBrowser = new DocsBrowser;
  _widgetEdit = new Edit;
  _widgetRegistration = new Registration;
  _widgetTextEditor = new TextEditor;

  // lo stacked prende l'ownership di queste finestre
  _stackedWidget->addWidget(_widgetLogin);
  _stackedWidget->addWidget(_widgetDocsBrowser);
  _stackedWidget->addWidget(_widgetEdit);
  _stackedWidget->addWidget(_widgetRegistration);
  _stackedWidget->addWidget(_widgetTextEditor);

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
  QObject::connect(_stackedWidget, SIGNAL(quit()), this, SLOT(closeStacked()));

  // alerts
  QObject::connect(_widgetLogin, &Login::alert, this, &GuiManager::alert);
  QObject::connect(_widgetDocsBrowser, &DocsBrowser::alert, this, &GuiManager::alert);
  QObject::connect(_widgetEdit, &Edit::alert, this, &GuiManager::alert);
  QObject::connect(_widgetRegistration, &Registration::alert, this, &GuiManager::alert);
  QObject::connect(_widgetTextEditor, &TextEditor::alert, this, &GuiManager::alert);

  // user/file updates
  QObject::connect(_user.get(), &User::iconChanged, _widgetDocsBrowser, &DocsBrowser::setIcon);
  QObject::connect(_user.get(), &User::iconChanged, _widgetEdit, &Edit::setIcon);
  QObject::connect(_user.get(), &User::iconChanged, _widgetTextEditor, &TextEditor::setIcon);

  //Login
  QObject::connect(_widgetLogin, &Login::login, this, &GuiManager::loginLogin);
  QObject::connect(_widgetLogin, &Login::signup, this, &GuiManager::loginSignup);

  //Registration
  QObject::connect(_widgetRegistration, &Registration::signup, this, &GuiManager::registrationSignup);
  QObject::connect(_widgetRegistration, &Registration::cancel, this, &GuiManager::registrationCancel);

  // Edit
  QObject::connect(_widgetEdit, &Edit::save, this, &GuiManager::editSave);
  QObject::connect(_widgetEdit, &Edit::cancel, this, &GuiManager::editCancel);
  QObject::connect(_widgetEdit, &Edit::deleteAccount, this, &GuiManager::editDelete);

  //DocsBrowser
  QObject::connect(_widgetDocsBrowser, &DocsBrowser::logout, this, &GuiManager::docsBrowserLogout);
  QObject::connect(_widgetDocsBrowser, &DocsBrowser::editAccount, this, &GuiManager::docsBrowserEditAccount);
  QObject::connect(_widgetDocsBrowser, &DocsBrowser::newFile, this, &GuiManager::docsBrowserNewFile);
  QObject::connect(_widgetDocsBrowser, &DocsBrowser::activateLink, this, &GuiManager::docsBrowserActivateLink);
  QObject::connect(_widgetDocsBrowser, &DocsBrowser::getDir, this, &GuiManager::getDirQuery);
  QObject::connect(_widgetDocsBrowser, &DocsBrowser::newDir, this, &GuiManager::newDirQuery);

  QObject::connect(this, &GuiManager::serverGetDirResponse, _widgetDocsBrowser, &DocsBrowser::showDir);
  QObject::connect(this, &GuiManager::serverNewDirResponse, _widgetDocsBrowser, &DocsBrowser::changeDir);

  //TextEditor
  QObject::connect(_widgetTextEditor, &TextEditor::share, this, &GuiManager::textEditorShare);
  QObject::connect(_widgetTextEditor, &TextEditor::logout, this, &GuiManager::textEditorLogout);
  QObject::connect(_widgetTextEditor, &TextEditor::close, this, &GuiManager::textEditorClose);
  QObject::connect(_widgetTextEditor, &TextEditor::edit, this, &GuiManager::textEditorEdit);
  QObject::connect(_widgetTextEditor, &TextEditor::remove, this, &GuiManager::textEditorRemove);
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

  QObject::connect(this, &GuiManager::newDirQuery, _manager.get(), &MessageManager::newDirQuery);
  QObject::connect(this, &GuiManager::editDirQuery, _manager.get(), &MessageManager::editDirQuery);
  QObject::connect(this, &GuiManager::deleteDirQuery, _manager.get(), &MessageManager::deleteDirQuery);
  QObject::connect(this, &GuiManager::getDirQuery, _manager.get(), &MessageManager::getDirQuery);
  QObject::connect(this, &GuiManager::moveFileQuery, _manager.get(), &MessageManager::moveFileQuery);

  //Signals from text editor to send directly to server
  QObject::connect(_widgetTextEditor, &TextEditor::getUserIcon, _manager.get(), &MessageManager::getUserIconQuery);
  QObject::connect(_widgetTextEditor, &TextEditor::commentLocalInsert, _manager.get(), &MessageManager::commentLocalInsertQuery);
  QObject::connect(_widgetTextEditor, &TextEditor::commentLocalUpdate, _manager.get(), &MessageManager::commentLocalUpdateQuery);
  QObject::connect(_widgetTextEditor, &TextEditor::commentLocalDelete, _manager.get(), &MessageManager::commentLocalDeleteQuery);
  QObject::connect(_widgetTextEditor, &TextEditor::localInsert, _manager.get(), &MessageManager::localInsertQuery);
  QObject::connect(_widgetTextEditor, &TextEditor::localDelete, _manager.get(), &MessageManager::localDeleteQuery);
  QObject::connect(_widgetTextEditor, &TextEditor::localUpdate, _manager.get(), &MessageManager::localUpdateQuery);
  QObject::connect(_widgetTextEditor, &TextEditor::localMove, _manager.get(), &MessageManager::localMoveQuery);
}

void GuiManager::connectServerToClient() {
  //Comunicazioni Server -> Client
  QObject::connect(_manager.get(), &MessageManager::errorResponse, this, &GuiManager::serverErrorResponse);

  QObject::connect(_manager.get(), &MessageManager::loginResponse, this, &GuiManager::serverLoginResponse);
  QObject::connect(_manager.get(), &MessageManager::newUserResponse, this, &GuiManager::serverNewUserResponse);
  QObject::connect(_manager.get(), &MessageManager::editUserResponse, this, &GuiManager::serverEditUserResponse);
  QObject::connect(_manager.get(), &MessageManager::deleteUserResponse, this, &GuiManager::serverDeleteUserResponse);

  QObject::connect(_manager.get(), &MessageManager::newFileResponse, this, &GuiManager::serverNewFileResponse);
  QObject::connect(_manager.get(), &MessageManager::activateLinkResponse, this, &GuiManager::serverActivateLinkResponse);
  QObject::connect(_manager.get(), &MessageManager::getDirResponse, this, &GuiManager::serverGetDirResponse);
  QObject::connect(_manager.get(), &MessageManager::newDirResponse, this, &GuiManager::serverNewDirResponse);
  QObject::connect(_manager.get(), &MessageManager::deleteFileResponse, this, &GuiManager::serverDeleteFileResponse);

  QObject::connect(_manager.get(), &MessageManager::getLinkResponse, this, &GuiManager::serverGetLinkResponse);

  //Signals to send directly to text editor
  QObject::connect(_manager.get(), &MessageManager::userConnectedQuery, _widgetTextEditor, &TextEditor::userConnected);
  QObject::connect(_manager.get(), &MessageManager::userDisconnectedQuery, _widgetTextEditor, &TextEditor::userDisconnected);
  QObject::connect(_manager.get(), &MessageManager::getIconResponse, _widgetTextEditor, &TextEditor::setUserIcon);
  QObject::connect(_manager.get(), &MessageManager::commentRemoteInsertQuery, _widgetTextEditor, &TextEditor::commentRemoteInsert);
  QObject::connect(_manager.get(), &MessageManager::commentRemoteUpdateQuery, _widgetTextEditor, &TextEditor::commentRemoteUpdate);
  QObject::connect(_manager.get(), &MessageManager::commentRemoteDeleteQuery, _widgetTextEditor, &TextEditor::commentRemoteDelete);
  QObject::connect(_manager.get(), &MessageManager::remoteInsertQuery, _widgetTextEditor, &TextEditor::remoteInsert);
  QObject::connect(_manager.get(), &MessageManager::remoteUpdateQuery, _widgetTextEditor, &TextEditor::remoteUpdate);
  QObject::connect(_manager.get(), &MessageManager::remoteDeleteQuery, _widgetTextEditor, &TextEditor::remoteDelete);
  QObject::connect(_manager.get(), &MessageManager::remoteMoveQuery, _widgetTextEditor, &TextEditor::remoteMove);
}

void GuiManager::connected() {
  info("Connesso al server");
  _connected = true;
  _stackedWidget->show();
}

void GuiManager::connectionLost() {
  alert(Alert::ERROR, "Connection lost.", "Connection error");
  emit quit();
}

void GuiManager::checkConnection() {
  if(_connected) return;

  alert(Alert::ERROR, "Cannot connect to the server.", "Connection error");
  emit quit();
}

void GuiManager::run() {
  info("GuiManager running");
  _serverThread->start();
  _managerThread->start();
  QTimer::singleShot(CONNECT_TIME_LIMIT, this, &GuiManager::checkConnection);
}

void GuiManager::closeStacked() {
  debug("Pressed close button");

  if(Confirm::show(_stackedWidget->currentWidget(), "Exit"))
    emit quit();
}

void GuiManager::alert(Alert type, const QString &what, const QString &title) {
  //debug("ALERT: " + QString::number(static_cast<int>(type)) + " " + title + " " + what);
  Info::show(_stackedWidget->currentWidget(), getAlertTitle(type), title, what);
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

void GuiManager::editDelete(const QString &token) {
  freezeWindow();
  emit deleteUserQuery(token);
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
  _user->setFileName(name);
  emit newFileQuery(token, name, dirId);
}

void GuiManager::docsBrowserActivateLink(const QString &token, const QString &link) {
  freezeWindow();
  emit activateLinkQuery(token, link);
}

void GuiManager::textEditorLogout(const QString &token) {
  emit logoutQuery(token);
  _user->logout();
  showWindow(_widgetLogin, true);
}

void GuiManager::textEditorShare(const QString &token, int fileId) {
  freezeWindow();
  emit getLinkQuery(token, fileId);
}

void GuiManager::textEditorClose(const QString &token, int fileId) {
  emit closeFileQuery(token, fileId);
  showWindow(_widgetDocsBrowser);
}

void GuiManager::textEditorEdit(const QString &token, int fileId, const std::optional<QString> &name) {
  emit editFileQuery(token, fileId, name);
}

void GuiManager::textEditorRemove(const QString &token, int fileId) {
  freezeWindow();
  emit deleteFileQuery(token, fileId);
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
  showWindow(_widgetDocsBrowser);
}

void GuiManager::serverDeleteUserResponse() {
  debug("GuiManager::serverDeleteUserResponse");
  unfreezeWindow();
  showWindow(_widgetLogin, true);
}

void GuiManager::serverNewFileResponse(int fileId) {
  debug("GuiManager::serverNewFileResponse");
  _user->openFile(fileId);
  _user->getFile()->addUser(_user->getUserId(), _user->getUsername());
  unfreezeWindow();
  showWindow(_widgetTextEditor, true);
}

void GuiManager::serverActivateLinkResponse(const FSElement &element, const File &file) {
  debug("GuiManager::serverActivateLinkResponse");
  //TODO questo FSElement va passato al docsbrowser che lo mette nella view
  _user->setFileName(element.getName());
  _user->openFile(element.getId(), file);
  unfreezeWindow();
  showWindow(_widgetTextEditor, true);
}

void GuiManager::serverDeleteFileResponse() {
  debug("GuiManager::serverDeleteFileResponse");
  //TODO rimuovere dalla view del docsbrowser
  _user->closeFile();
  unfreezeWindow();
  showWindow(_widgetDocsBrowser);
}

void GuiManager::serverGetLinkResponse(const QString &link) {
  debug("GuiManager::serverGetLinkResponse");
  unfreezeWindow();
  alert(Alert::INFO, link, GET_LINK_TITLE);
}
