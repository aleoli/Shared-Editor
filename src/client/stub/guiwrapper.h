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

public slots:
    // slot di azioni compiute dall'user
    void sendLoginRequest(QString username, QString psw);
    void sendFileRequest(int id);
    void sendNewFileRequest();

    // slot di ricezione messaggi dal mm
    void errorReceived(QString why);
    void loginResponseReceived(QString token);
    void fileResponseReceived(File *f); //TODO questo non è un puntatore ma un oggetto, messo così per evitare errori
    void newFileResponseReceived(int id);

private:
    void connectWidgets();
    void connectMM();
    void checkWaiting(bool shouldBe);

    //ENUM per tenere traccia di quale finestra è aperta al momento (serve per sapere dove stampare i msg di errore)
    //si può anche usare per rinforzare i controlli sui msg ricevuti
    enum class OpenWindow {
        NONE,
        LOGIN,
        REGISTRATION,
        BROWSER,
        EDITOR
    };

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
