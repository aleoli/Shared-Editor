#ifndef MAINTHREAD_H
#define MAINTHREAD_H

#include <QWidget>
#include "login.h"
#include "fileselector.h"
#include "TextEdit.h"
#include "../message_manager.h"
#include "Message.h"
#include <memory>

class GuiWrapper : public QWidget
{
    Q_OBJECT

    typedef struct {
        QString username;
        QString psw;
        QString token;
    } account;

public:
  GuiWrapper(QWidget *parent = 0);
  ~GuiWrapper();
  void run();

signals:
  //segnali da emettere per mandare messaggi al server
  void sendLoginQuery(QString username, QString password);
  void sendNewUserQuery(QString username, QString password, QString passwordRepeat);
  void sendNewFileQuery(QString token, QString name, int dirId = 0);
  void sendGetFileQuery(QString token, int fileId);
  void sendCloseFileQuery(QString token, int fileId);
  void sendLocalInsertQuery(QString token, int fileId, std::vector<Symbol> symbols);
  void sendLocalDeleteQuery(QString token, int fileId, std::vector<SymbolId> ids);
  void sendLocalUpdateQuery(QString token, int fileId, std::vector<Symbol> symbols);
  void sendLocalMoveQuery(QString token, int fileId, SymbolId symbolId, int cursorPosition);

  //segnali per il TextEdit
  void remoteInsertQuery(int fileId, std::vector<Symbol> symbols);
  void remoteDeleteQuery(int fileId, std::vector<SymbolId> ids);
  void remoteUpdateQuery(int fileId, std::vector<Symbol> symbols);
  void userConnectedQuery(int fileId, int clientId, QString username);
  void userDisconnectedQuery(int fileId, int clientId);
  void remoteMoveQuery(int fileId, int clientId, SymbolId symbolId, int cursorPosition);

public slots:
  // slot di azioni compiute dall'user (connessioni con gli elementi grafici)
  void loginQuery(QString username, QString psw);
  void newUserQuery(QString username, QString psw, QString pswRepeat);

  void newFileQuery();
  void getFileQuery(int id);

  void closeFileQuery(int fileId);
  void localInsertQuery(int fileId, std::vector<Symbol> symbols);
  void localDeleteQuery(int fileId, std::vector<SymbolId> ids);
  void localUpdateQuery(int fileId, std::vector<Symbol> symbols);
  void localMoveQuery(int fileId, SymbolId symbolId, int cursorPosition);

  // slot di ricezione messaggi dal mm
  void errorResponseReceived(QString reason);
  void loginResponseReceived(QString token, int userId, QString nickname, QString icon);
  void newUserResponseReceived(QString token, int userId);
  void newFileResponseReceived(int fileId);
  void getFileResponseReceived(File file, int charId);
  void remoteInsertQueryReceived(int fileId, std::vector<Symbol> symbols);
  void remoteDeleteQueryReceived(int fileId, std::vector<SymbolId> ids);
  void remoteUpdateQueryReceived(int fileId, std::vector<Symbol> symbols);
  void userConnectedQueryReceived(int fileId, int clientId, QString username);
  void userDisconnectedQueryReceived(int fileId, int clientId);
  void remoteMoveQueryReceived(int fileId, int clientId, SymbolId symbolId, int cursorPosition);

private:
  void connectWidgets();
  void initClientToServer();
  void initServerToClient();
  void checkWaiting(bool shouldBe);
  void testWindows();

  //ENUM per tenere traccia di quale finestra è aperta al momento (serve per sapere dove stampare i msg di errore)
  //si può anche usare per rinforzare i controlli sui msg ricevuti
  enum class OpenWindow {
      NONE,
      LOGIN,
      REGISTRATION,
      BROWSER,
      EDITOR
  };

  //Attributo per tenere traccia dell'ultimo messaggio inviato
  // è uguale a TYPE * 100 + ACTION
  // ulteriore check per garantire sicurezza / stabilità all'applicazione
  // (i msg tipo localinsert e altri non devono essere contati, solo quelli che prevedono risposta)
  //INFO considerare se inserirlo anche nel client definitivo
  int _lastMessageSent;

  //booleano per vedere se sono in attesa di una risposta a un messaggio. Usato per rinforzare i check sui msg inviati
  //se è impostato a true e voglio mandare un messaggio -> errore grave!
  //NB non contano i messaggi di tipo FILE_EDIT
  //per far ciò che questo funzioni, quando sono in attesa di un msg la gui si deve "freezare" in attesa, magari facendo
  //vedere un indicatore di caricamento
  //il booleano è settato quando mando un msg, e resettato quando arriva la risposta
  bool _waiting;

  // finestre
  Login *_login;
  FileSelector *_fileSelector;
  TextEdit *_textEdit;

  //altro
  account _account;
  OpenWindow _window;
  std::shared_ptr<MessageManager> _manager;
};

#endif // MAINTHREAD_H
