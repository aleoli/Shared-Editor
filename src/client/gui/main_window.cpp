#include "main_window.h"

#include "utils.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent), _user(User::get()) {
    initLoading();
}

void MainWindow::closeEvent(QCloseEvent *bar) {
  emit exit();
  bar->ignore();
}

void MainWindow::initLoading() {
  _movie = std::make_shared<QMovie>(":/imgs/loading");

  _loading = new QLabel(this);
  _loading->setMovie(_movie.get());
  _loading->setAlignment(Qt::AlignCenter);
}

void MainWindow::freeze() {
  setEnabled(false);
  _movie->start();
  _loading->move(this->rect().center() - _loading->rect().center());
  _loading->raise();
  _loading->show();
}

void MainWindow::unfreeze() {
  setEnabled(true);
  _loading->hide();
  _movie->stop();
}
