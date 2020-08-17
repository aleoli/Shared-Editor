#include "docsbrowser.h"
#include "../ui/ui_docsbrowser.h"

#include "utils.h"

DocsBrowser::DocsBrowser(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DocsBrowser),
    _user(User::get())
{
    ui->setupUi(this);

    _widgetAccount = findChild<QPushButton *>("btn_logout");
    _widgetNewFile = findChild<QPushButton *>("btn_new_file");
    _actionLogout= findChild<QAction *>("actionLogout");

    connect(_widgetAccount, &QPushButton::clicked, this, &DocsBrowser::_account);
    connect(_widgetNewFile, &QPushButton::clicked, this, &DocsBrowser::_newFile);
    connect(_actionLogout, &QAction::triggered, this, &DocsBrowser::_logout);
}

DocsBrowser::~DocsBrowser()
{
    delete ui;
}

void DocsBrowser::clear() {
  // TODO Cose da fare quando la finestra è portata in primo piano (es. svuotare i campi)
  setIcon();
}

void DocsBrowser::setIcon() {
  _widgetAccount->setIcon(_user->getIcon());
  _widgetAccount->setIconSize(_widgetAccount->size());
}

void DocsBrowser::_account(bool checked) {
  //TODO qua dovrebbe comparire un menu a tendina con le opzioni edit account e logout
  emit editAccount();
  //emit logout(_user->getToken());
}

void DocsBrowser::_newFile(bool checked) {
  //TODO qua dovrebbe comparire un menù a tendina chiedendo il nome del file
  //TODO inoltre bisogna ricavare l'id della cartella che lo conterrà
  emit newFile(_user->getToken(), "originalFile");
}

void DocsBrowser::_logout(bool checked) {
  emit logout(_user->getToken());
}
