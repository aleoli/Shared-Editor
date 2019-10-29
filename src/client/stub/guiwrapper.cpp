#include "guiwrapper.h"

#include <iostream>
#include <QTimer>
#include "errordialog.h"
#include "exceptions.h"

using namespace se_exceptions;

#define USERLOGIN static_cast<int>(Message::Type::USER) * 100 + static_cast<int>(Message::UserAction::LOGIN)
#define USERNEW static_cast<int>(Message::Type::USER) * 100 + static_cast<int>(Message::UserAction::NEW)
#define FILENEW static_cast<int>(Message::Type::FILE) * 100 + static_cast<int>(Message::FileAction::NEW)
#define FILEGET static_cast<int>(Message::Type::FILE) * 100 + static_cast<int>(Message::FileAction::GET)

GuiWrapper::GuiWrapper(QWidget *parent)
    : QWidget(parent)
{
  _login = new Login(this);
  _fileSelector = new FileSelector(this);
  _textEdit = new TextEdit(this);

  _textEdit->setAttribute(Qt::WA_QuitOnClose, false);

  _manager = MessageManager::get();

  _window = OpenWindow::NONE;
  _waiting = false;

  debug(QString::number(USERLOGIN));

  connectWidgets();
  initClientToServer();
  initServerToClient();
}

GuiWrapper::~GuiWrapper()
{
}

void GuiWrapper::run() {
  _window = OpenWindow::LOGIN;
  _login->show();
  //_fileSelector->show();
  //_textEdit->show();

  //TODO poi rimuovi o commenta
  testWindows();
}

void GuiWrapper::testWindows() {
  info("TEST FINESTRE");
  info("Tra 15 secondi arriva una loginResponseReceived");
  info("Tra 30 secondi arriva un newFileResponseReceived");

  QTimer::singleShot(15000, this, [this]() {loginResponseReceived("questoeuntokenautentico", 33, nullptr, nullptr);});
  QTimer::singleShot(30000, this, [this]() {newFileResponseReceived(0);});
}

void GuiWrapper::connectWidgets() {
  // connetto i signal dei widget con gli slot di questa classe
  connect(_login, &Login::loginQuery, this, &GuiWrapper::loginQuery);
  connect(_login, &Login::newUserQuery, this, &GuiWrapper::newUserQuery);

  connect(_fileSelector, &FileSelector::newFileQuery, this, &GuiWrapper::newFileQuery);
  connect(_fileSelector, &FileSelector::getFileQuery, this, &GuiWrapper::getFileQuery);

  connect(_textEdit, &TextEdit::closeFileQuery, this, &GuiWrapper::closeFileQuery);
  //TODO altri del textEdit
}

void GuiWrapper::initClientToServer() {
  //Comunicazioni Client -> Server
  connect(this, &GuiWrapper::sendLoginQuery, _manager.get(), &MessageManager::loginQuery);
  connect(this, &GuiWrapper::sendNewUserQuery, _manager.get(), &MessageManager::newUserQuery);
  connect(this, &GuiWrapper::sendNewFileQuery, _manager.get(), &MessageManager::newFileQuery);
  connect(this, &GuiWrapper::sendGetFileQuery, _manager.get(), &MessageManager::getFileQuery);
  connect(this, &GuiWrapper::sendCloseFileQuery, _manager.get(), &MessageManager::closeFileQuery);
  connect(this, &GuiWrapper::sendLocalInsertQuery, _manager.get(), &MessageManager::localInsertQuery);
  connect(this, &GuiWrapper::sendLocalDeleteQuery, _manager.get(), &MessageManager::localDeleteQuery);
  connect(this, &GuiWrapper::sendLocalUpdateQuery, _manager.get(), &MessageManager::localUpdateQuery);
  connect(this, &GuiWrapper::sendLocalMoveQuery, _manager.get(), &MessageManager::localMoveQuery);
}

void GuiWrapper::initServerToClient() {
  //Comunicazioni Server -> Client
  connect(_manager.get(), &MessageManager::errorResponse, this, &GuiWrapper::errorResponseReceived);
  connect(_manager.get(), &MessageManager::loginResponse, this, &GuiWrapper::loginResponseReceived);
  connect(_manager.get(), &MessageManager::newUserResponse, this, &GuiWrapper::newUserResponseReceived);
  connect(_manager.get(), &MessageManager::newFileResponse, this, &GuiWrapper::newFileResponseReceived);
  connect(_manager.get(), &MessageManager::getFileResponse, this, &GuiWrapper::getFileResponseReceived);
  connect(_manager.get(), &MessageManager::remoteInsertQuery, this, &GuiWrapper::remoteInsertQueryReceived);
  connect(_manager.get(), &MessageManager::remoteDeleteQuery, this, &GuiWrapper::remoteDeleteQueryReceived);
  connect(_manager.get(), &MessageManager::remoteUpdateQuery, this, &GuiWrapper::remoteUpdateQueryReceived);
  connect(_manager.get(), &MessageManager::userConnectedQuery, this, &GuiWrapper::userConnectedQueryReceived);
  connect(_manager.get(), &MessageManager::userDisconnectedQuery, this, &GuiWrapper::userDisconnectedQueryReceived);
  connect(_manager.get(), &MessageManager::remoteMoveQuery, this, &GuiWrapper::remoteMoveQueryReceived);
}

void GuiWrapper::checkWaiting(bool shouldBe) {
  if(_waiting != shouldBe) {
    error("L'attributo _waiting è settato a " + QString::number(_waiting)
      + " ma dovrebbe essere " + QString::number(shouldBe));

    //TODO eccezione
    throw SE_Exception("errore _waiting");
  }

  // setto anche il nuovo valore
  _waiting = !_waiting;
}

void GuiWrapper::loginQuery(QString username, QString psw) {
  checkWaiting(false);
  _lastMessageSent = USERLOGIN;

  info("Invio login request");

  _account.username = username;
  _account.psw = psw;
  _account.token = nullptr;

  emit sendLoginQuery(username, psw);
}

void GuiWrapper::newUserQuery(QString username, QString psw, QString pswRepeat) {
  checkWaiting(false);
  _lastMessageSent = USERNEW;

  info("Invio new user request");

  _account.username = username;
  _account.psw = psw;
  _account.token = nullptr;

  emit sendNewUserQuery(username, psw, pswRepeat);
}

void GuiWrapper::getFileQuery(int id) {
  checkWaiting(false);
  _lastMessageSent = FILEGET;

  info("Invio get file request");

  emit sendGetFileQuery(_account.token, id);
}

void GuiWrapper::newFileQuery() {
  checkWaiting(false);
  _lastMessageSent = FILENEW;

  info("Invio new file request");

  //TODO il nome del file nel client definitivo deve essere ricevuto come parametro e salvato da qualche parte
  emit sendNewFileQuery(_account.token, "questoeunnomeautentico");
}

void GuiWrapper::closeFileQuery(int fileId) {
  info("Invio close file request");

  emit sendCloseFileQuery(_account.token, fileId);

  // riapro browser
  _window = OpenWindow::BROWSER;
  _fileSelector->show();
}

void GuiWrapper::localInsertQuery(int fileId, std::vector<Symbol> symbols) {
  //TODO
}

void GuiWrapper::localDeleteQuery(int fileId, std::vector<SymbolId> ids) {
  //TODO
}

void GuiWrapper::localUpdateQuery(int fileId, std::vector<Symbol> symbols) {
  //TODO
}

void GuiWrapper::localMoveQuery(int fileId, SymbolId symbolId, int cursorPosition) {
  //TODO
}

// SLOT MM

void GuiWrapper::errorResponseReceived(QString reason) {
  checkWaiting(true);

  info("Ricevuta risposta di errore");
  debug(reason);

  switch(_window) {
      case OpenWindow::LOGIN:
          ErrorDialog::showDialog(_login, reason);
          break;

      case OpenWindow::EDITOR:
          ErrorDialog::showDialog(_textEdit, reason);
          break;

      case OpenWindow::BROWSER:
          ErrorDialog::showDialog(_fileSelector, reason);
          break;

      case OpenWindow::REGISTRATION:
          //ErrorDialog::showDialog(_registration, reason);
          break;

      default:
          //TODO eccezione?
          break;
  }
}

void GuiWrapper::loginResponseReceived(QString token, int userId, QString nickname, QString icon) {
  checkWaiting(true);
  if(_lastMessageSent != USERLOGIN) {
      debug("Last message: " + QString::number(_lastMessageSent) + " Response received: " + QString::number(USERLOGIN));
      throw SE_Exception("Errore, mi aspettavo un messaggio diverso");
  }

  info("Ricevuto login response");
  debug("Token: " + token);
  debug("User id: " + QString::number(userId));

  _account.token = token;
  _textEdit->setUserId(userId);
  //TODO nickname e icona...

  _login->unblock();
  _login->clear();
  _login->close();

  _window = OpenWindow::BROWSER;
  _fileSelector->show();
}

void GuiWrapper::newUserResponseReceived(QString token, int userId) {
  checkWaiting(true);
  if(_lastMessageSent != USERNEW) {
      debug("Last message: " + QString::number(_lastMessageSent) + " Response received: " + QString::number(USERLOGIN));
      throw SE_Exception("Errore, mi aspettavo un messaggio diverso");
  }

  info("Ricevuto new user response");
  debug("Token: " + token);
  debug("User id: " + QString::number(userId));

  _account.token = token;
  _textEdit->setUserId(userId);

  _login->unblock();
  _login->clear();
  _login->close();

  _window = OpenWindow::BROWSER;
  _fileSelector->show();
}

void GuiWrapper::newFileResponseReceived(int id) {
  checkWaiting(true);
  if(_lastMessageSent != FILENEW) {
      debug("Last message: " + QString::number(_lastMessageSent) + " Response received: " + QString::number(USERLOGIN));
      throw SE_Exception("Errore, mi aspettavo un messaggio diverso");
  }

  info("Ricevuto new file response");

  _fileSelector->unblock();
  _fileSelector->close();
  //TODO nella versione definitiva, qua bisogna aggiornare la view del browser

  File f(id);
  _textEdit->setFile(f);

  _window = OpenWindow::EDITOR;
  _textEdit->show();
}

void GuiWrapper::getFileResponseReceived(File file, int charId) {
  checkWaiting(true);
  if(_lastMessageSent != FILEGET) {
      debug("Last message: " + QString::number(_lastMessageSent) + " Response received: " + QString::number(USERLOGIN));
      throw SE_Exception("Errore, mi aspettavo un messaggio diverso");
  }

  info("Ricevuto get file response");

  _fileSelector->unblock();
  _fileSelector->close();

  _textEdit->setFile(file, charId);

  _window = OpenWindow::EDITOR;
  _textEdit->show();
}

void GuiWrapper::remoteInsertQueryReceived(int fileId, std::vector<Symbol> symbols) {
  info("Ricevuto remote insert query");
  //TODO signal per il textEdit
}

void GuiWrapper::remoteDeleteQueryReceived(int fileId, std::vector<SymbolId> ids) {
  info("Ricevuto remote delete query");
  //TODO signal per il textEdit
}

void GuiWrapper::remoteUpdateQueryReceived(int fileId, std::vector<Symbol> symbols) {
  info("Ricevuto remote update query");
  //TODO signal per il textEdit
}

void GuiWrapper::userConnectedQueryReceived(int fileId, int clientId, QString username) {
  info("Ricevuto user connected query");
  //TODO signal per il textEdit
}

void GuiWrapper::userDisconnectedQueryReceived(int fileId, int clientId) {
  info("Ricevuto user disconnected query");
  //TODO signal per il textEdit
}

void GuiWrapper::remoteMoveQueryReceived(int fileId, int clientId, SymbolId symbolId, int cursorPosition) {
  info("Ricevuto remote move query");
  //TODO signal per il textEdit
}
