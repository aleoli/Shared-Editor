#pragma once

#include <QObject>
#include <QCloseEvent>
#include <QSize>

#include <vector>

#include "main_window.h"

class StackedWidget : public QObject {
  Q_OBJECT

public:
  StackedWidget(QObject *parent = nullptr);
  ~StackedWidget();

  void show();
  void hide();
  bool close();

  void addWidget(MainWindow *widget);
  void setCurrentWidget(MainWindow *widget);
  MainWindow *currentWidget() const;

signals:
  void quit();

private:
  void setCurrent(MainWindow *widget);

  std::vector<MainWindow *> _widgets;
  MainWindow *_current;
  bool _show;
};
