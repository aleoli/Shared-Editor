#include "manager.h"
#include "QObject"
#include "login.h"
#include "record.h"
#include "docsbrowser.h"
#include "texteditor.h"
#include "mainwindow.h"
#include <iostream>
//#include <string>
#include <QThread>
#include "errordialog.h"
#include "exceptions.h"
#include "def.h"

using namespace se_exceptions;

#define USERLOGIN static_cast<int>(Message::Type::USER) * 100 + static_cast<int>(Message::UserAction::LOGIN)
#define USERNEW static_cast<int>(Message::Type::USER) * 100 + static_cast<int>(Message::UserAction::NEW)
#define FILENEW static_cast<int>(Message::Type::FILE) * 100 + static_cast<int>(Message::FileAction::NEW)
#define FILEGET static_cast<int>(Message::Type::FILE) * 100 + static_cast<int>(Message::FileAction::GET)

//per finalità di debug e testing
#define CONNECT_TO_SERVER 1
#define USE_TESTS 0

shared_ptr<Manager> Manager::instance = nullptr;

Manager::Manager() {}

Manager::Manager(const SysConf &conf, QObject *parent)
    : QObject(parent)
{

    MainWindow w{};
    w.show();

    // inizializzazione threads
  initThreads(conf);

  // inizializzazione finestre
  /*_landing = new Landing(this);
  login = new Login(this);
  docsBrowser = new DocsBrowser(this);
  _textEdit = new TextEdit(this);
  _textEdit->setAttribute(Qt::WA_QuitOnClose, false);

  _window = OpenWindow::NONE;
  _waiting = false;
*/
  // connessione signal / slot della gui
  connectWidgets();
  initClientToServer();
  initServerToClient();
}

Manager::~Manager()
{
  _serverThread->quit();
  _managerThread->quit();

  _serverThread->wait();
  _managerThread->wait();
}

void Manager::run() {
#if CONNECT_TO_SERVER
  _serverThread->start();
  _managerThread->start();

 // _window = OpenWindow::LANDING;
 // _landing->show();
#else
  login->show();
  //docsBrowser->show();
  //_textEdit->setUser(33, "bob");
  //_textEdit->show();
#endif

#if USE_TESTS
  //testWindows();
  //testEditor();
  //testCRDT();
#endif
}

void Manager::initThreads(const SysConf &conf) {
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

void Manager::connected() {
  info("Connesso al server");
  this->mw->showLoginPage();
  /*_window = OpenWindow::LOGIN;
  _landing->close();
  login->show();*/
}

void Manager::connectionLost() {
  warn("La connessione è stata persa");

  //TODO display messaggio su schermo (?)

  //TODO provare a recuperare la connessione invece di chiudere (?)
  emit quit();
}

void Manager::initClientToServer() {
  //Comunicazioni Client -> Server
  connect(this, &Manager::sendLoginQuery, _manager.get(), &MessageManager::loginQuery);
  connect(this, &Manager::sendNewUserQuery, _manager.get(), &MessageManager::newUserQuery);
  connect(this, &Manager::sendNewFileQuery, _manager.get(), &MessageManager::newFileQuery);
  connect(this, &Manager::sendGetFileQuery, _manager.get(), &MessageManager::getFileQuery);
  connect(this, &Manager::sendCloseFileQuery, _manager.get(), &MessageManager::closeFileQuery);
  connect(this, &Manager::sendLocalInsertQuery, _manager.get(), &MessageManager::localInsertQuery);
  connect(this, &Manager::sendLocalDeleteQuery, _manager.get(), &MessageManager::localDeleteQuery);
  connect(this, &Manager::sendLocalUpdateQuery, _manager.get(), &MessageManager::localUpdateQuery);
  connect(this, &Manager::sendLocalMoveQuery, _manager.get(), &MessageManager::localMoveQuery);
}

void Manager::initServerToClient() {
  //Comunicazioni Server -> Client
  connect(_manager.get(), &MessageManager::errorResponse, this, &Manager::errorResponseReceived);
  connect(_manager.get(), &MessageManager::loginResponse, this, &Manager::loginResponseReceived);
  connect(_manager.get(), &MessageManager::newUserResponse, this, &Manager::newUserResponseReceived);
  connect(_manager.get(), &MessageManager::newFileResponse, this, &Manager::newFileResponseReceived);
  connect(_manager.get(), &MessageManager::getFileResponse, this, &Manager::getFileResponseReceived);
  connect(_manager.get(), &MessageManager::remoteInsertQuery, this, &Manager::remoteInsertQueryReceived);
  connect(_manager.get(), &MessageManager::remoteDeleteQuery, this, &Manager::remoteDeleteQueryReceived);
  connect(_manager.get(), &MessageManager::remoteUpdateQuery, this, &Manager::remoteUpdateQueryReceived);
  connect(_manager.get(), &MessageManager::userConnectedQuery, this, &Manager::userConnectedQueryReceived);
  connect(_manager.get(), &MessageManager::userDisconnectedQuery, this, &Manager::userDisconnectedQueryReceived);
  connect(_manager.get(), &MessageManager::remoteMoveQuery, this, &Manager::remoteMoveQueryReceived);
}

void Manager::connectWidgets() {
  // login
  connect(login, &Login::access, this, &Manager::checkLogin);
  //record
  connect(record, &Record::record_try, this, &Manager::checkRecord);

  // browser file
  connect(docsBrowser, &DocsBrowser::newFileQuery, this, &Manager::newFileQuery);
  connect(docsBrowser, &DocsBrowser::getFileQuery, this, &Manager::getFileQuery);
  connect(docsBrowser, &DocsBrowser::newFolderQuery, this, &Manager::newFolderQuery);
  connect(docsBrowser, &DocsBrowser::getFolderQuery, this, &Manager::getFolderQuery);

  // TextEdit -> Manager (Client -> Server)
  connect(textEditor, &TextEditor::closeFileQuery, this, &Manager::closeFileQuery);
  connect(textEditor, &TextEditor::localInsertQuery, this, &Manager::localInsertQuery);
  connect(textEditor, &TextEditor::localDeleteQuery, this, &Manager::localDeleteQuery);
  connect(textEditor, &TextEditor::localUpdateQuery, this, &Manager::localUpdateQuery);
  connect(textEditor, &TextEditor::localMoveQuery, this, &Manager::localMoveQuery);

  // Manager -> TextEdit (Server -> Client)
  connect(this, &Manager::setTEFile, textEditor, &TextEditor::setFile);
  connect(this, &Manager::remoteInsertQuery, textEditor, &TextEditor::remoteInsertQuery);
  connect(this, &Manager::remoteDeleteQuery, textEditor, &TextEditor::remoteDeleteQuery);
  connect(this, &Manager::remoteUpdateQuery, textEditor, &TextEditor::remoteUpdateQuery);
  connect(this, &Manager::userConnectedQuery, textEditor, &TextEditor::userConnectedQuery);
  connect(this, &Manager::userDisconnectedQuery, textEditor, &TextEditor::userDisconnectedQuery);
  connect(this, &Manager::remoteMoveQuery, textEditor, &TextEditor::remoteMoveQuery);
}

shared_ptr<Manager> Manager::get(const SysConf conf) {
    if(instance == nullptr) {
        instance.reset(new Manager{conf, nullptr});
    }
    return instance;
}

void Manager::exitView(QWidget *page) {
    this->_pages.push_back(page);
}


/*
string Manager::getUsername() const
{
    return username;
}

void Manager::setUsername(const string &value)
{
    username = value;
}

string Manager::getPassword() const
{
    return password;
}

void Manager::setPassword(const string &value)
{
    password = value;
}
*/

void Manager::checkLogin(QString username, QString password)
{
    if(logged || this->main_user!=nullptr){
        //print alert already logged user, wanna login into another account?
        if(true)
            this->forceLogOut();
        else{
            return;//capire che fare
        }
    }
    main_user=new Account(username,password);
    emit this->sendLoginQuery(username,password);
}

void Manager::loginResponseReceived(QString token, int userId, QString nickname, QString icon)
{
    main_user->setServerResponse(token,userId,nickname,icon);
    logged=true;
    mw->showDocsBrowserPage();
}

void Manager::checkRecord(QString username, QString password, QString password_repeat) //bisognerebbe aggiungere nickname
{
    if(logged || this->main_user!=nullptr){
        //print alert already logged user, wanna login into another account?
        if(true)
            this->forceLogOut();
        else{
            return; //capire che fare
        }
    }
    main_user = new Account(username,password);
    emit sendNewUserQuery(username,password,password_repeat);
}


void Manager::newUserResponseReceived(QString token, int userId) {
    //TODO must add nickname and icon
    QString nickname = "bob";
    QString icon = "hey, I'm a picture";
    main_user->setServerResponse(token,userId,nickname,icon);
    logged=true;
    mw->showDocsBrowserPage();
}



void Manager::forceLogOut()
{
    emit logOut();
    emit quit();
    logged=false;
    main_user=nullptr;
    mw->showLoginPage();
}

void Manager::forceClose()
{
    emit logOut();
    emit quit();
    logged=false;
    main_user=nullptr;
    mw->close();
}

void Manager::setFeasibleActions()
{
    mw->showFeasibleActions();
}

bool Manager::checkRight()
{
    return (logged && main_user!=nullptr);
}

void Manager::isLogged() {
    emit isLogged(logged);
}



void Manager::back() {
    auto prev = this->_pages.back();
    //mw->setCentralWidget(prev);
    this->_pages.pop_back();
    emit this->setCentral(prev);
    cout << "sono qua" << endl;
}


void Manager::errorResponseReceived(QString reason) {
    //checkWaiting(true);

    info("Ricevuta risposta di errore");
    debug(reason);
    ErrorDialog::showDialog(mw,reason);
}

void Manager::newFileQuery(QString filename) {
    /*checkWaiting(false);*/
    //TODO
    //_lastMessageSent = FILENEW;

    info("Invio new file query");

    //INFO il nome del file nel client definitivo deve essere ricevuto come parametro e salvato da qualche parte*/
    emit sendNewFileQuery(main_user->getToken(), filename);
}
void Manager::getFileQuery(int id) {
    //checkWaiting(false);
    //TODO
    //_lastMessageSent = FILEGET;

    info("Invio get file query");

    emit sendGetFileQuery(main_user->getToken(), id);
}
void Manager::newFolderQuery(QString foldername) {
    //checkWaiting(false);
    //TODO
    //_lastMessageSent = FOLDERNEW;

    info("Invio new folder query");

    emit sendNewFolderQuery(main_user->getToken(), foldername);
}
void Manager::getFolderQuery(QString foldername) {
    /*checkWaiting(false);*/
    //TODO
    //_lastMessageSent = OPENFOLDER;

    info("Invio open folder query");

    //INFO il nome del file nel client definitivo deve essere ricevuto come parametro e salvato da qualche parte*/
    emit sendOpenFolderQuery(main_user->getToken(), foldername);

}

void Manager::getFileResponseReceived(File file, int charId) {
    //checkWaiting(true);
    //TODO
    /*if(_lastMessageSent != FILEGET) {
        debug("Last message: " + QString::number(_lastMessageSent) + " Response received: " + QString::number(FILEGET));
        throw SE_Exception("Errore, mi aspettavo un messaggio diverso");
    }*/

    info("Ricevuto get file response");
    debug("Char id: " + QString::number(charId));
/*
    _fileSelector->unblock();
    _fileSelector->close();

    textEditor->setFile(file, charId);

    _window = OpenWindow::EDITOR;
    _textEdit->show();*/
    emit setTEFile(file, charId);
    this->mw->showTextEditorPage();
}

void Manager::closeFileQuery(int fileId) {
    //TODO
    //_lastMessageSent
    info("Invio close file query");

    emit sendCloseFileQuery(main_user->getToken(), fileId);

    // riapro browser
    mw->showDocsBrowserPage();
}

void Manager::localInsertQuery(int fileId, std::vector<Symbol> symbols) {
    //debug("Invio local insert query");

    emit sendLocalInsertQuery(main_user->getToken(), fileId, symbols);
}

void Manager::localDeleteQuery(int fileId, std::vector<SymbolId> ids) {
    //debug("Invio local delete query");

    emit sendLocalDeleteQuery(main_user->getToken(), fileId, ids);
}

void Manager::localUpdateQuery(int fileId, std::vector<Symbol> symbols) {
    //debug("Invio local update query");

    emit sendLocalUpdateQuery(main_user->getToken(), fileId, symbols);
}

void Manager::localMoveQuery(int fileId, SymbolId symbolId, int cursorPosition) {
    //debug("Invio local move query");

    emit sendLocalMoveQuery(main_user->getToken(), fileId, symbolId, cursorPosition);
}
void Manager::newFileResponseReceived(int id) {
    //checkWaiting(true);
    /*if(_lastMessageSent != FILENEW) {
        debug("Last message: " + QString::number(_lastMessageSent) + " Response received: " + QString::number(FILENEW));
        throw SE_Exception("Errore, mi aspettavo un messaggio diverso");
    }*/

    info("Ricevuto new file response");
    debug("File id: " + QString::number(id));

    //_fileSelector->unblock();
    //_fileSelector->close();
    //INFO nella versione definitiva, qua GuiWrapperbisogna aggiornare la view del browser

    File f(id);
    this->setTEFile(f,id);

    /*_window = OpenWindow::EDITOR;
    _textEdit->show();*/
    this->mw->showTextEditorPage();
}
void Manager::remoteInsertQueryReceived(int fileId, int clientId, std::vector<Symbol> symbols) {
    //debug("Ricevuto remote insert query");

    emit remoteInsertQuery(fileId, clientId, symbols);
}

void Manager::remoteDeleteQueryReceived(int fileId, int clientId, std::vector<SymbolId> ids) {
    //debug("Ricevuto remote delete query");

    emit remoteDeleteQuery(fileId, clientId, ids);
}

void Manager::remoteUpdateQueryReceived(int fileId, int clientId, std::vector<Symbol> symbols) {
    //debug("Ricevuto remote update query");

    emit remoteUpdateQuery(fileId, clientId, symbols);
}

void Manager::userConnectedQueryReceived(int fileId, int clientId, QString username) {
    info("Ricevuto user connected query");
    debug("Client id: " + QString::number(clientId));

    emit userConnectedQuery(fileId, clientId, username);
}

void Manager::userDisconnectedQueryReceived(int fileId, int clientId) {
    info("Ricevuto user disconnected query");
    debug("Client id: " + QString::number(clientId));

    emit userDisconnectedQuery(fileId, clientId);
}

void Manager::remoteMoveQueryReceived(int fileId, int clientId, SymbolId symbolId, int cursorPosition) {
    debug("Ricevuto remote move query");
    debug("Client id: " + QString::number(clientId));

    emit remoteMoveQuery(fileId, clientId, symbolId, cursorPosition);
}



/*void Manager::initGUI(){

    login=mw->getLogin();
    record=mw->getRecord();
    docsBrowser=mw->getDocsBrowser();
    textEditor=mw->getTextEditor();
    mw->setManagerClass(this);
    login->setManagerClass(this);
    record->setManagerClass(this);
    docsBrowser->setManagerClass(this);
    textEditor->setManagerClass(this);
    for(currentPageIndex=0;currentPageIndex<10;currentPageIndex++)
        pages[currentPageIndex]=-1;
}*/

