#pragma once

#include <QListWidgetItem>

class RemoteUser : public QObject {
  Q_OBJECT

public:
  RemoteUser(QObject *parent = nullptr);

private:
  int _userId;

};
