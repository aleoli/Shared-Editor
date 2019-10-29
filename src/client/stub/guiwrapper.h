#ifndef MAINTHREAD_H
#define MAINTHREAD_H

#include <QWidget>
#include "login.h"
#include "fileselector.h"
#include "TextEdit.h"
#include "../message_manager.h"
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
  void sendLoginRequest(QString username, QString password);
  void sendNewUserRequest(QString username, QString password, QString passwordRepeat);
  void sendNewFileRequest(QString token, QString name, int dirId = 0);
  void sendGetFileRequest(QString token, int fileId);
  void sendCloseFileRequest(QString token, int fileId);
  void sendLocalInsertRequest(QString token, int fileId, std::vector<Symbol> symbols);
  void sendLocalDeleteRequest(QString token, int fileId, std::vector<SymbolId> ids);
  void sendLocalUpdateRequest(QString token, int fileId, std::vector<Symbol> symbols);
  void sendLocalMoveRequest(QString token, int fileId, SymbolId symbolId, int cursorPosition);

public slots:
  // slot di azioni compiute dall'user (connessioni con gli elementi grafici)
  void loginRequest(QString username, QString psw);
  void newUserRequest(QString username, QString psw, QString pswRepeat);

  void newFileRequest();
  void getFileRequest(int id);
  //TODO quelli del TextEdit
  void closeFileRequest();

  // slot di ricezione messaggi dal mm
  void errorResponseReceived(QString reason);
  void loginResponseReceived(QString token, int userId, QString nickname, QString icon);
  void newUserResponseReceived(QString token, int userId);
  void newFileResponseReceived(int fileId);
  void getFileResponseReceived(File file, int charId);
  void remoteInsertQueryReceived(std::vector<Symbol> symbols);
  void remoteDeleteQueryReceived(std::vector<SymbolId> ids);
  void remoteUpdateQueryReceived(std::vector<Symbol> symbols);
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

  //TODO considerare la presenza di un altro enum per tenere traccia dell'ultimo messaggio inviato
  // ulteriore check per garantire sicurezza / stabilità all'applicazione
  // (i msg tipo localinsert e altri non devono essere contati, solo quelli che prevedono risposta)

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
