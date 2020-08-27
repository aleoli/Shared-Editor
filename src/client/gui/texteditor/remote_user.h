#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QString>
#include <QColor>
#include <QIcon>

#include "cursor.h"
#include "item_widget.h"

namespace Ui {
class ConnectedUser;
}

class RemoteUser : public ItemWidget
{
    Q_OBJECT

public:
  RemoteUser(int userId, const QString &username, const QColor &color, Cursor *cursor = nullptr, bool online = true);
  ~RemoteUser();

public slots:
  int getUserId() const;
  void setIcon(const QIcon &icon);
  QIcon getIcon() const;
  void setOnline(bool online);
  QColor getColor() const;
  Cursor *getCursor() const;

private slots:

private:
  void setName(const QString &name);

  Ui::ConnectedUser *ui;
  QLabel *_widgetName;
  QPushButton *_widgetIcon;

  int _userId;
  QString _username;
  QIcon _icon;
  QColor _color;
  bool _online;
  Cursor *_cursor;
};
