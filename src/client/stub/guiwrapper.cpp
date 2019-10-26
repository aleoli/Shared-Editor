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

    _manager = MessageManager::get();

    _window = OpenWindow::NONE;
    _waiting = false;

    connectWidgets();
    connectMM();
}

GuiWrapper::~GuiWrapper()
{
}

void GuiWrapper::run() {
    _window = OpenWindow::LOGIN;
    //_login->show();
    _textEdit->show();

    //TODO rimuovi
    //QTimer::singleShot(5000, this, [this]() {_login->close();});
    //QTimer::singleShot(5000, this, [this]() {ErrorDialog::showDialog(_login, "ciao");});
}

void GuiWrapper::connectWidgets() {
    // connetto i signal dei widget con gli slot di questa classe
    connect(_login, &Login::loginRequest, this, &GuiWrapper::sendLoginRequest);
    connect(_fileSelector, &FileSelector::openFileRequest, this, &GuiWrapper::sendFileRequest);
    connect(_fileSelector, &FileSelector::newFileRequest, this, &GuiWrapper::sendNewFileRequest);
}

void GuiWrapper::connectMM() {
    // TODO connetto i signal del MM (per ricezione dei messaggi) con gli slot di questa classe
}

void GuiWrapper::sendLoginRequest(QString username, QString psw) {
    // TODO qui si dovrebbe creare un Message e mandarlo al server
    checkWaiting(false);

    _account.username = username;
    _account.psw = psw;
    _account.token = nullptr;
}

void GuiWrapper::sendFileRequest(int id) {
    // TODO qui si dovrebbe creare un Message e mandarlo al server
    checkWaiting(false);

    (void) id;
}

void GuiWrapper::sendNewFileRequest() {
    // TODO qui si dovrebbe creare un Message e mandarlo al server
    checkWaiting(false);

    //QString name = rndString(64);
    //TODO devo salvare il nome del file da qualche parte perchè nella risposta non c'è
}

void GuiWrapper::errorReceived(QString why) {
    checkWaiting(true);

    switch(_window) {
        case OpenWindow::LOGIN:
            ErrorDialog::showDialog(_login, why);
            break;

        case OpenWindow::EDITOR:
            //ErrorDialog::showDialog(_textEdit, why);
            break;

        case OpenWindow::BROWSER:
            ErrorDialog::showDialog(_fileSelector, why);
            break;

        case OpenWindow::REGISTRATION:
            //ErrorDialog::showDialog(_registration, why);
            break;

        default:
            std::cout << "Errore:" << why.toStdString() << std::endl;
            //TODO eccezione?
    }
}

void GuiWrapper::loginResponseReceived(QString token) {
    checkWaiting(true);

    _account.token = token;
    _login->unblock();
    _login->clear();
    _login->close();

    _window = OpenWindow::BROWSER;
    _fileSelector->show();
}

void GuiWrapper::fileResponseReceived(File *f) {
    checkWaiting(true);

    //TODO assegna file
    //_file = f;
    _fileSelector->unblock();
    _fileSelector->close();
    //TODO magari aggiorno la view del browser inserendo il nuovo file

    //TODO apri editor con file
    _window = OpenWindow::EDITOR;
}

void GuiWrapper::newFileResponseReceived(int id) {
    checkWaiting(true);

    _fileSelector->unblock();
    _fileSelector->close();
    //TODO magari aggiorno la view del browser inserendo il nuovo file

    //TODO creo file vuoto con questo id e apro editor
    (void) id;
}

void GuiWrapper::checkWaiting(bool shouldBe) {
    if(_waiting != shouldBe) {
        //TODO eccezione
        throw std::runtime_error("errore _waiting");
    }

    // setto anche il nuovo valore
    _waiting = !_waiting;
}
