#include "fileselector.h"
#include "ui_fileselector.h"

#include <iostream>
#include <QTimer>
#include "utils.h"
#include "errordialog.h"

FileSelector::FileSelector(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::FileSelector)
{
  ui->setupUi(this);

  _id = findChild<QLineEdit *>("fileIdForm");
  _btnOpen = findChild<QPushButton *>("openButton");
  _btnNew = findChild<QPushButton *>("newButton");

  initLoading();

  //QTimer::singleShot(5000, this, [this]() {unblock();});
}

FileSelector::~FileSelector()
{
  delete ui;
}

void FileSelector::initLoading() {
  _movie = std::make_shared<QMovie>(":/imgs/loading");

  _loading = new QLabel(this);
  _loading->setMovie(_movie.get());
  _loading->setAlignment(Qt::AlignCenter);

  _loading->move(this->rect().center() - _loading->rect().center());
}

void FileSelector::unblock() {
  stopLoading();
  enable();
}

void FileSelector::getFile() {
  bool ok;
  auto id = _id->text().toInt(&ok);

  if(!ok) {
    debug("Premuto btnOpen. Errore form");
    ErrorDialog::showDialog(this, "Inserisci un fileId corretto!");
    return;
  }

  disable();
  startLoading();

  debug("Premuto btnOpen. Id: " + QString::number(id));

  emit getFileRequest(id);
}

void FileSelector::newFile() {
  disable();
  startLoading();

  debug("Premuto btnNew");

  emit newFileRequest();
}

void FileSelector::disable() {
  _id->setEnabled(false);
  _btnOpen->setEnabled(false);
  _btnNew->setEnabled(false);
}

void FileSelector::enable() {
  _id->setEnabled(true);
  _btnOpen->setEnabled(true);
  _btnNew->setEnabled(true);
}

void FileSelector::clear() {
  _id->clear();
}

void FileSelector::startLoading() {
  _movie->start();
  _loading->show();
}

void FileSelector::stopLoading() {
  _loading->hide();
  _movie->stop();
}
