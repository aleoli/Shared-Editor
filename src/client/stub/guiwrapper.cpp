#include "guiwrapper.h"

#include <iostream>
#include <QTimer>
#include "errordialog.h"

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

  QTimer::singleShot(15000, this, [this]() {loginResponseReceived("questoeuntokenautentico");});
  QTimer::singleShot(30000, this, [this]() {newFileResponseReceived(0);});
}

void GuiWrapper::connectWidgets() {
  // connetto i signal dei widget con gli slot di questa classe
  connect(_login, &Login::loginRequest, this, &GuiWrapper::loginRequest);
  connect(_login, &Login::newUserRequest, this, &GuiWrapper::newUserRequest);

  connect(_fileSelector, &FileSelector::newFileRequest, this, &GuiWrapper::newFileRequest);
  connect(_fileSelector, &FileSelector::getFileRequest, this, &GuiWrapper::getFileRequest);

  connect(_textEdit, &TextEdit::closeFileRequest, this, &GuiWrapper::closeFileRequest);
}

void GuiWrapper::initClientToServer() {
  //Comunicazioni Client -> Server
  //TODO collego SIGNAL GuiWrapper a SLOT MessageManager
}

void GuiWrapper::initServerToClient() {
  //Comunicazioni Server -> Client
  //TODO collego SIGNAL MessageManager a SLOT GuiWrapper
}

void GuiWrapper::checkWaiting(bool shouldBe) {
  if(_waiting != shouldBe) {
    error("L'attributo _waiting è settato a " + QString::number(_waiting)
      + " ma dovrebbe essere " + QString::number(shouldBe));

    //TODO eccezione
    throw std::runtime_error("errore _waiting");
  }

  // setto anche il nuovo valore
  _waiting = !_waiting;
}

void GuiWrapper::loginRequest(QString username, QString psw) {
  // TODO qui si dovrebbe creare un Message e mandarlo al server
  checkWaiting(false);

  info("Invio login request");

  _account.username = username;
  _account.psw = psw;
  _account.token = nullptr;
}

void GuiWrapper::newUserRequest(QString username, QString psw, QString pswRepeat) {
  // TODO qui si dovrebbe creare un Message e mandarlo al server
  checkWaiting(false);

  info("Invio new user request");

  _account.username = username;
  _account.psw = psw;
  _account.token = nullptr;
}

void GuiWrapper::getFileRequest(int id) {
  // TODO qui si dovrebbe creare un Message e mandarlo al server
  checkWaiting(false);

  info("Invio get file request");

  (void) id;
}

void GuiWrapper::newFileRequest() {
  // TODO qui si dovrebbe creare un Message e mandarlo al server
  checkWaiting(false);

  info("Invio new file request");

  //QString name = rndString(64);
  //TODO devo salvare il nome del file da qualche parte perchè nella risposta non c'è
}

void GuiWrapper::closeFileRequest() {
  // TODO qui si dovrebbe creare un Message e mandarlo al server

  info("Invio close file request");

  // riapro browser
  _window = OpenWindow::BROWSER;
  _fileSelector->show();
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
          //ErrorDialog::showDialog(_textEdit, reason);
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

void GuiWrapper::loginResponseReceived(QString token) {
  checkWaiting(true);

  info("Ricevuto login response");

  _account.token = token;
  _login->unblock();
  _login->clear();
  _login->close();

  _window = OpenWindow::BROWSER;
  _fileSelector->show();
}

void GuiWrapper::newUserResponseReceived(QString token) {
  //TODO
}

void GuiWrapper::newFileResponseReceived(int id) {
  checkWaiting(true);

  info("Ricevuto new file response");

  _fileSelector->unblock();
  _fileSelector->close();
  //TODO magari aggiorno la view del browser inserendo il nuovo file

  //TODO creo file vuoto con questo id e apro editor
  File f(id);
  _textEdit->setFile(f);

  _window = OpenWindow::EDITOR;
  _textEdit->show();
}

void GuiWrapper::getFileResponseReceived(File file, int charId) {
  checkWaiting(true);

  info("Ricevuto file response");

  //TODO assegna file
  //_file = f;
  _fileSelector->unblock();
  _fileSelector->close();
  //TODO magari aggiorno la view del browser inserendo il nuovo file

  //TODO apri editor con file
  _window = OpenWindow::EDITOR;
}

void GuiWrapper::remoteInsertQueryReceived(std::vector<Symbol> symbols) {
  //TODO
}

void GuiWrapper::remoteDeleteQueryReceived(std::vector<SymbolId> ids) {
  //TODO
}

void GuiWrapper::remoteUpdateQueryReceived(std::vector<Symbol> symbols) {
  //TODO
}

void GuiWrapper::userConnectedQueryReceived(int fileId, int clientId, QString username) {
  //TODO
}

void GuiWrapper::userDisconnectedQueryReceived(int fileId, int clientId) {
  //TODO
}

void GuiWrapper::remoteMoveQueryReceived(int fileId, int clientId, SymbolId symbolId, int cursorPosition) {
  //TODO
}
