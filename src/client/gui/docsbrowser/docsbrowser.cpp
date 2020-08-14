#include "docsbrowser.h"
#include "../ui/ui_docsbrowser.h"

DocsBrowser::DocsBrowser(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DocsBrowser),
    _user(User::get())
{
    ui->setupUi(this);

    _widgetLogout = findChild<QPushButton *>("btn_logout");

    connect(_widgetLogout, &QPushButton::clicked, this, &DocsBrowser::_logout);
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
  _widgetLogout->setIcon(_user->getIcon());
  _widgetLogout->setIconSize(_widgetLogout->size());
}

void DocsBrowser::_logout(bool checked) {
  emit logout(_user->getToken());
}
