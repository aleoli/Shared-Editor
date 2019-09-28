#pragma once

#include <QApplication>
#include <QRect>
#include <QDesktopWidget>

#include "TextEdit.h"

void prova_textedit(QApplication &a) {
  // classe che conterrà tutta la logica della nostra interfaccia.. DA IMPLEMENTARE!
  TextEdit mw;

  // setto dimensioni e posizione della finestra in base al desktop
  const QRect availableGeometry = QApplication::desktop()->availableGeometry(&mw);
  mw.resize(availableGeometry.width() / 2, (availableGeometry.height() * 2) / 3);
  mw.move((availableGeometry.width() - mw.width()) / 2,
          (availableGeometry.height() - mw.height()) / 2);

  // eseguo
  mw.show();
  exit(a.exec());
}
