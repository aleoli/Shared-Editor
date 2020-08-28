#include "remote_user.h"
#include "../ui/ui_texteditor_connectedUser.h"

#include "utils.h"
#include "image_utils.h"
#include "exceptions.h"

using namespace se_exceptions;

RemoteUser::RemoteUser(int userId, const QString &username, const QColor &color, Cursor *cursor, bool online) :
  ItemWidget(),
  ui(new Ui::ConnectedUser),
  _userId(userId),
  _username(username),
  _color(color),
  _cursor(cursor),
  _online(online)
{
  ui->setupUi(this);

  _widgetIcon = ui->label_icon;
  _widgetName = ui->label_name;

  _widgetName->setStyleSheet("QLabel {color : " + _color.name(QColor::HexArgb) + "; }");
  setName(_username);

  setIcon(image_utils::loadImage(DEFAULT_ICON));

  _item->setSizeHint(this->size());
}

RemoteUser::~RemoteUser()
{
    delete ui;
}

int RemoteUser::getUserId() const {
  return _userId;
}

void RemoteUser::setIcon(const QIcon &icon) {
  if(icon.isNull()) return;

  auto size = _widgetIcon->size();
  _widgetIcon->setIcon(icon);
  _widgetIcon->setIconSize(QSize(size.width()-2, size.height()-2));
  _icon = icon;
}

QIcon RemoteUser::getIcon() const {
  return _icon;
}

void RemoteUser::setOnline(bool online) {
  _online = online;
}

QColor RemoteUser::getColor() const {
  return _color;
}

Cursor *RemoteUser::getCursor() const {
  return _cursor;
}

void RemoteUser::setName(const QString &name) {
  if(name.length() > MAX_SIZE_NAME) {
    auto substr = name.mid(0, MAX_SIZE_NAME);
    substr.append("...");
    if(!_cursor) substr.append(" (you)");
    _widgetName->setText(substr);
    _widgetName->setToolTip(name);
  }
  else {
    auto str = name;
    if(!_cursor) str.append(" (you)");
    _widgetName->setText(str);
  }
}
