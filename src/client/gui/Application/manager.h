#ifndef MANAGER_H
#define MANAGER_H
#include <QWidget>
#include <list>
#include "document.h"
#include <QMainWindow>

/*#include "login.h"
#include "record.h"
#include "docsbrowser.h"
#include "texteditor.h"
#include "mainwindow.h"*/
#include <memory>
#include <QWidget>
#include <memory>

//#include "landing.h"
#include "../login/login.h"
//#include "fileselector.h"
//#include "TextEdit.h"
#include "../../message_manager.h"
#include "../../server.h"
#include "../../sys.h"
#include "../Message.h"

class Account;
class Document;
class Login;
class Record;
class DocsBrowser;
class TextEditor;
class MainWindow;
using namespace std;




class Manager : public QObject
{
    Q_OBJECT

public:
    Manager(const SysConf &conf, QObject *parent = 0);
    virtual ~Manager();


    static shared_ptr<Manager> get(const SysConf);

    string getUsername() const;
    void setUsername(const string &value);
    string getPassword() const;
    void setPassword(const string &value);
    //altri threads
    std::shared_ptr<MessageManager> _manager;
    std::shared_ptr<Server> _server;
    QThread *_serverThread, *_managerThread;


    //void setGUI(QMainWindow mw, Login l, TextEditor te, Record r, DocsBrowser db);

    //MainWindow *getMw() const;
    //void setMw(MainWindow *value);

    void exitView(QWidget *);

private:
    explicit Manager();
    //Gui elements
    MainWindow *mw;
    Login *login;
    TextEditor *textEditor;
    Record *record;
    DocsBrowser *docsBrowser;
    //DocWidget *d;
    void connectWidgets();
    void initGUI();

    //gestione pagine
    void nextPage();
    void previousPage();
    void openedNewPage();

    //int pages[10];
    list<QWidget*> _pages;
    //int currentPageIndex=0;
    //int actions[10];

    static shared_ptr<Manager> instance;

    //user account
    Account *main_user;
    bool logged;

    //user accessible documents
    list<Document> users_docs;

    //user credentials
    QString username,password;

    //actions
    void forceLogOut();
    void forceClose();
    void setFeasibleActions();

    //check legit action
    bool checkRight();


    void initClientToServer();
    void initServerToClient();

    void initThreads(const SysConf &conf);
    void run();

//    void connected();
//    void connectionLost();

public slots:
    void back();
    void isLogged();
    void connected();
    void connectionLost();

    //void loginQuery(QString username, QString psw);
    //void newUserQuery(QString username, QString psw, QString pswRepeat);
    void checkLogin(QString username,QString password);
    void checkRecord(QString nickname,QString username, QString password);

  void newFileQuery(QString filename);
  void getFileQuery(int id);
  void closeFileQuery(int fileId);
  void newFolderQuery(QString foldername);
  void getFolderQuery(QString foldername);
  void localInsertQuery(int fileId, std::vector<Symbol> symbols);
  void localDeleteQuery(int fileId, std::vector<SymbolId> ids);
  void localUpdateQuery(int fileId, std::vector<Symbol> symbols);
  void localMoveQuery(int fileId, SymbolId symbolId, int cursorPosition);

  // slot di ricezione messaggi dal mm*/
  void errorResponseReceived(QString reason);
  void loginResponseReceived(QString token, int userId, QString nickname, QString icon);
  void newUserResponseReceived(QString token, int userId);
  void newFileResponseReceived(int fileId);
  void getFileResponseReceived(File file, int charId);
  void remoteInsertQueryReceived(int fileId, int clientId, std::vector<Symbol> symbols);
  void remoteDeleteQueryReceived(int fileId, int clientId, std::vector<SymbolId> ids);
  void remoteUpdateQueryReceived(int fileId, int clientId, std::vector<Symbol> symbols);
  void userConnectedQueryReceived(int fileId, int clientId, QString username);
  void userDisconnectedQueryReceived(int fileId, int clientId);
  void remoteMoveQueryReceived(int fileId, int clientId, SymbolId symbolId, int cursorPosition);


signals:
    void isLogged(bool logged);
    void setCentral(QWidget *w);
    void loginSuccessful();
    void logOut();

    //messaggi al server
    void sendLoginQuery(QString username, QString password);
    void sendNewUserQuery(QString username, QString password, QString passwordRepeat);
    void sendNewFileQuery(QString token, QString name, int dirId = 0);
    void sendGetFileQuery(QString token, int fileId);
    void sendCloseFileQuery(QString token, int fileId);
    void sendNewFolderQuery(QString token, QString folder);
    void sendOpenFolderQuery(QString token, QString folder);
    void sendLocalInsertQuery(QString token, int fileId, std::vector<Symbol> symbols);
    void sendLocalDeleteQuery(QString token, int fileId, std::vector<SymbolId> ids);
    void sendLocalUpdateQuery(QString token, int fileId, std::vector<Symbol> symbols);
    void sendLocalMoveQuery(QString token, int fileId, SymbolId symbolId, int cursorPosition);

    //segnali per il TextEdit
      void remoteInsertQuery(int fileId, int clientId, std::vector<Symbol> symbols);
      void remoteDeleteQuery(int fileId, int clientId, std::vector<SymbolId> ids);
      void remoteUpdateQuery(int fileId, int clientId, std::vector<Symbol> symbols);
      void userConnectedQuery(int fileId, int clientId, QString username);
      void userDisconnectedQuery(int fileId, int clientId);
      void remoteMoveQuery(int fileId, int clientId, SymbolId symbolId, int cursorPosition);

      void quit();
      void setTEFile(File f, int charId);

private slots:

    //void openDocument(DocWidget *d);

    //altro
  //account _account;
  //OpenWindow _window;

};

#endif // MANAGER_H
