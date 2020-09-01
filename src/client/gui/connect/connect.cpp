#include "connect.h"
#include "../ui/ui_dialog_connect.h"

#include "utils.h"
#include "alert_messages.h"
#include "user.h"
#include "exceptions.h"

using namespace se_exceptions;

Connect::Connect(QWidget *parent) :
    MainWindow(parent),
    ui(new Ui::Connect)
{
  ui->setupUi(this);

  _widgetAddress = ui->ledit_IP;
  _widgetPort = ui->ledit_Port;
  _widgetConnect = ui->btn_yes;
  _widgetExit = ui->btn_no;

  connect(_widgetConnect, &QPushButton::clicked, this, &Connect::_connect);
  connect(_widgetExit, &QPushButton::clicked, this, &Connect::_exit);
}

Connect::~Connect()
{
    delete ui;
}

void Connect::clear() {
  warn("Connect::clear called");
}

void Connect::setDefaultAddress(const QString &host, int port) {
  _widgetAddress->setText(host);
  _widgetPort->setText(QString::number(port));
}

QString Connect::getHost() const {
  return _widgetAddress->text();
}

int Connect::getPort() {
  bool ok;
  auto port = _widgetPort->text().toInt(&ok);

  if(!ok || port < 1 || port > 65535) {
    return -1;
  }

  return port;
}

void Connect::_connect(bool checked) {
  auto port = getPort();
  if(port == -1) {
    emit alert(Alert::ERROR, WRONG_PORT);
    return;
  }

  emit openConnection(getHost(), port);
}

void Connect::_exit(bool checked) {
  emit quit();
}
