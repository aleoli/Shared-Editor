#include "main_window.h"

#include "utils.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent), _user(User::get()) {}

void MainWindow::freeze() {
  debug("Freeze");
  //TODO
}

void MainWindow::unfreeze() {
  debug("Unfreeze");
  //TODO
}
