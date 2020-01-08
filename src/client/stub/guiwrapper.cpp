#include "guiwrapper.h"

#include <QThread>

#include "errordialog.h"
#include "exceptions.h"
#include "def.h"

using namespace se_exceptions;

#define USERLOGIN static_cast<int>(Message::Type::USER) * 100 + static_cast<int>(Message::UserAction::LOGIN)
#define USERNEW static_cast<int>(Message::Type::USER) * 100 + static_cast<int>(Message::UserAction::NEW)
#define FILENEW static_cast<int>(Message::Type::FILE) * 100 + static_cast<int>(Message::FileAction::NEW)
#define FILEGET static_cast<int>(Message::Type::FILE) * 100 + static_cast<int>(Message::FileAction::GET)
#define FILELINK static_cast<int>(Message::Type::FILE) * 100 + static_cast<int>(Message::FileAction::ACTIVATE_LINK)

//per finalità di debug e testing
#define CONNECT_TO_SERVER 1
#define USE_TESTS 0

GuiWrapper::GuiWrapper(const SysConf &conf, QWidget *parent)
    : QWidget(parent)
{
  // inizializzazione threads
  initThreads(conf);

  // inizializzazione finestre
  _landing = new Landing(this);
  _login = new Login(this);
  _fileSelector = new FileSelector(this);
  _textEdit = new TextEdit(this);
  _textEdit->setAttribute(Qt::WA_QuitOnClose, false);

  _window = OpenWindow::NONE;
  _waiting = false;

  // connessione signal / slot della gui
  connectWidgets();
  initClientToServer();
  initServerToClient();
}

GuiWrapper::~GuiWrapper()
{
  _serverThread->quit();
  _managerThread->quit();

  _serverThread->wait();
  _managerThread->wait();
}

void GuiWrapper::run() {
#if CONNECT_TO_SERVER
  _serverThread->start();
  _managerThread->start();

  _window = OpenWindow::LANDING;
  _landing->show();
#else
  //_login->show();
  //_fileSelector->show();
  _textEdit->setUser(33, "bob");
  _textEdit->show();
#endif

#if USE_TESTS
  //testWindows();
  //testEditor();
  //testCRDT();
#endif
}

void GuiWrapper::initThreads(const SysConf &conf) {
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

void GuiWrapper::connected() {
  info("Connesso al server");

  _window = OpenWindow::LOGIN;
  _landing->close();
  _login->show();
}

void GuiWrapper::connectionLost() {
  warn("La connessione è stata persa");

  //TODO display messaggio su schermo (?)

  //TODO provare a recuperare la connessione invece di chiudere (?)
  emit quit();
}

void GuiWrapper::connectWidgets() {
  // login
  connect(_login, &Login::loginQuery, this, &GuiWrapper::loginQuery);
  connect(_login, &Login::newUserQuery, this, &GuiWrapper::newUserQuery);

  // browser file
  connect(_fileSelector, &FileSelector::newFileQuery, this, &GuiWrapper::newFileQuery);
  connect(_fileSelector, &FileSelector::getFileQuery, this, &GuiWrapper::getFileQuery);
  connect(_fileSelector, &FileSelector::activateLinkQuery, this, &GuiWrapper::activateLinkQuery);

  // TextEdit -> GuiWrapper (Client -> Server)
  connect(_textEdit, &TextEdit::closeFileQuery, this, &GuiWrapper::closeFileQuery);
  connect(_textEdit, &TextEdit::localInsertQuery, this, &GuiWrapper::localInsertQuery);
  connect(_textEdit, &TextEdit::localDeleteQuery, this, &GuiWrapper::localDeleteQuery);
  connect(_textEdit, &TextEdit::localUpdateQuery, this, &GuiWrapper::localUpdateQuery);
  connect(_textEdit, &TextEdit::localMoveQuery, this, &GuiWrapper::localMoveQuery);
  connect(_textEdit, &TextEdit::getLinkQuery, this, &GuiWrapper::getLinkQuery);

  // GuiWrapper -> TextEdit (Server -> Client)
  connect(this, &GuiWrapper::remoteInsertQuery, _textEdit, &TextEdit::remoteInsertQuery);
  connect(this, &GuiWrapper::remoteDeleteQuery, _textEdit, &TextEdit::remoteDeleteQuery);
  connect(this, &GuiWrapper::remoteUpdateQuery, _textEdit, &TextEdit::remoteUpdateQuery);
  connect(this, &GuiWrapper::userConnectedQuery, _textEdit, &TextEdit::userConnectedQuery);
  connect(this, &GuiWrapper::userDisconnectedQuery, _textEdit, &TextEdit::userDisconnectedQuery);
  connect(this, &GuiWrapper::remoteMoveQuery, _textEdit, &TextEdit::remoteMoveQuery);
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
  connect(this, &GuiWrapper::sendGetLinkQuery, _manager.get(), &MessageManager::getLinkQuery);
  connect(this, &GuiWrapper::sendActivateLinkQuery, _manager.get(), &MessageManager::activateLinkQuery);
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
  connect(_manager.get(), &MessageManager::getLinkResponse, this, &GuiWrapper::getLinkResponseReceived);
  connect(_manager.get(), &MessageManager::activateLinkResponse, this, &GuiWrapper::activateLinkResponseReceived);
}

void GuiWrapper::checkWaiting(bool shouldBe) {
  if(_waiting != shouldBe) {
    error("L'attributo _waiting è settato a " + QString::number(_waiting)
      + " ma dovrebbe essere " + QString::number(shouldBe));

    throw SE_Exception("errore _waiting");
  }

  // setto anche il nuovo valore
  _waiting = !_waiting;
}

void GuiWrapper::loginQuery(QString username, QString psw) {
  checkWaiting(false);
  _lastMessageSent = USERLOGIN;

  info("Invio login query");

  _account.username = username;
  _account.psw = psw;
  _account.userId = -1;
  _account.token = nullptr;

  emit sendLoginQuery(username, psw);
}

void GuiWrapper::newUserQuery(QString username, QString psw, QString pswRepeat) {
  checkWaiting(false);
  _lastMessageSent = USERNEW;

  info("Invio new user query");

  _account.username = username;
  _account.psw = psw;
  _account.userId = -1;
  _account.token = nullptr;

  emit sendNewUserQuery(username, psw, pswRepeat);
}

void GuiWrapper::getFileQuery(int id) {
  checkWaiting(false);
  _lastMessageSent = FILEGET;

  info("Invio get file query");

  //setto file ID per quando arriverà la risposta e aprirò l'editor
  _textEdit->setFileId(id);

  emit sendGetFileQuery(_account.token, id);
}

void GuiWrapper::activateLinkQuery(QString link) {
  checkWaiting(false);
  _lastMessageSent = FILELINK;

  info("Invio activate link query");

  emit sendActivateLinkQuery(_account.token, link);
}

void GuiWrapper::newFileQuery() {
  checkWaiting(false);
  _lastMessageSent = FILENEW;

  info("Invio new file query");

  //INFO il nome del file nel client definitivo deve essere ricevuto come parametro e salvato da qualche parte
  emit sendNewFileQuery(_account.token, "questoeunnomeautentico");
}

void GuiWrapper::closeFileQuery(int fileId) {
  info("Invio close file query");

  emit sendCloseFileQuery(_account.token, fileId);

  // riapro browser
  _window = OpenWindow::BROWSER;
  _fileSelector->show();
}

void GuiWrapper::localInsertQuery(int fileId, std::vector<Symbol> symbols) {
  //debug("Invio local insert query");

  emit sendLocalInsertQuery(_account.token, fileId, symbols);
}

void GuiWrapper::localDeleteQuery(int fileId, std::vector<SymbolId> ids) {
  //debug("Invio local delete query");

  emit sendLocalDeleteQuery(_account.token, fileId, ids);
}

void GuiWrapper::localUpdateQuery(int fileId, std::vector<Symbol> symbols) {
  //debug("Invio local update query");

  emit sendLocalUpdateQuery(_account.token, fileId, symbols);
}

void GuiWrapper::localMoveQuery(int fileId, SymbolId symbolId, int cursorPosition) {
  //debug("Invio local move query");

  emit sendLocalMoveQuery(_account.token, fileId, symbolId, cursorPosition);
}

void GuiWrapper::getLinkQuery(int fileId) {
  //debug("Invio share query");

  emit sendGetLinkQuery(_account.token, fileId);
}

// SLOT MM

void GuiWrapper::errorResponseReceived(QString reason) {
  checkWaiting(true);

  info("Ricevuta risposta di errore");
  debug(reason);

  switch(_window) {
    case OpenWindow::LANDING:
      ErrorDialog::showDialog(_landing, reason);
      break;

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
      throw SE_Exception{"Nessuna finestra aperta, oppure OpenWindow mal settato"};
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
  _account.userId = userId;
  _textEdit->setUser(userId, _account.username);
  //INFO nella versione definitiva settare nickname e icona

  _login->unblock();
  _login->clear();
  _login->close();

  _window = OpenWindow::BROWSER;
  _fileSelector->show();
}

void GuiWrapper::newUserResponseReceived(QString token, int userId) {
  checkWaiting(true);
  if(_lastMessageSent != USERNEW) {
      debug("Last message: " + QString::number(_lastMessageSent) + " Response received: " + QString::number(USERNEW));
      throw SE_Exception("Errore, mi aspettavo un messaggio diverso");
  }

  info("Ricevuto new user response");
  debug("Token: " + token);
  debug("User id: " + QString::number(userId));

  _account.token = token;
  _account.userId = userId;
  _textEdit->setUser(userId, _account.username);

  _login->unblock();
  _login->clear();
  _login->close();

  _window = OpenWindow::BROWSER;
  _fileSelector->show();
}

void GuiWrapper::newFileResponseReceived(int id) {
  checkWaiting(true);
  if(_lastMessageSent != FILENEW) {
      debug("Last message: " + QString::number(_lastMessageSent) + " Response received: " + QString::number(FILENEW));
      throw SE_Exception("Errore, mi aspettavo un messaggio diverso");
  }

  info("Ricevuto new file response");
  debug("File id: " + QString::number(id));

  _fileSelector->unblock();
  _fileSelector->close();
  //INFO nella versione definitiva, qua bisogna aggiornare la view del browser

  File f;
  _textEdit->setFile(f);
  _textEdit->setFileId(id);

  _window = OpenWindow::EDITOR;
  _textEdit->show();
}

void GuiWrapper::getFileResponseReceived(File file, int charId) {
  checkWaiting(true);
  if(_lastMessageSent != FILEGET) {
      debug("Last message: " + QString::number(_lastMessageSent) + " Response received: " + QString::number(FILEGET));
      throw SE_Exception("Errore, mi aspettavo un messaggio diverso");
  }

  info("Ricevuto get file response");
  debug("Char id: " + QString::number(charId));

  _fileSelector->unblock();
  _fileSelector->close();

  _textEdit->setFile(file, charId);
  //l'id è settato quando faccio la query

  _window = OpenWindow::EDITOR;
  _textEdit->show();
}

void GuiWrapper::remoteInsertQueryReceived(int fileId, int clientId, std::vector<Symbol> symbols) {
  //debug("Ricevuto remote insert query");

  emit remoteInsertQuery(fileId, clientId, symbols);
}

void GuiWrapper::remoteDeleteQueryReceived(int fileId, int clientId, std::vector<SymbolId> ids) {
  //debug("Ricevuto remote delete query");

  emit remoteDeleteQuery(fileId, clientId, ids);
}

void GuiWrapper::remoteUpdateQueryReceived(int fileId, int clientId, std::vector<Symbol> symbols) {
  //debug("Ricevuto remote update query");

  emit remoteUpdateQuery(fileId, clientId, symbols);
}

void GuiWrapper::userConnectedQueryReceived(int fileId, int clientId, QString username) {
  info("Ricevuto user connected query");
  debug("Client id: " + QString::number(clientId));

  emit userConnectedQuery(fileId, clientId, username);
}

void GuiWrapper::userDisconnectedQueryReceived(int fileId, int clientId) {
  info("Ricevuto user disconnected query");
  debug("Client id: " + QString::number(clientId));

  emit userDisconnectedQuery(fileId, clientId);
}

void GuiWrapper::remoteMoveQueryReceived(int fileId, int clientId, SymbolId symbolId, int cursorPosition) {
  debug("Ricevuto remote move query");
  debug("Client id: " + QString::number(clientId));

  emit remoteMoveQuery(fileId, clientId, symbolId, cursorPosition);
}

void GuiWrapper::getLinkResponseReceived(QString link) {
  debug("Ricevuto share link");
  debug("Link: " + link);
}

void GuiWrapper::activateLinkResponseReceived(FSElement element, File file) {
  checkWaiting(true);
  if(_lastMessageSent != FILELINK) {
      debug("Last message: " + QString::number(_lastMessageSent) + " Response received: " + QString::number(FILELINK));
      throw SE_Exception("Errore, mi aspettavo un messaggio diverso");
  }

  debug("Ricevuto activate link response");

  _fileSelector->unblock();
  _fileSelector->close();

  _textEdit->setFile(file);
  _textEdit->setFileId(element.getId());

  _window = OpenWindow::EDITOR;
  _textEdit->show();
}
