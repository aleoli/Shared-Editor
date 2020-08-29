#pragma once

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QPlainTextEdit>
#include <QDateTime>
#include <QColor>
#include <QString>

#include "Identifier.h"
#include "File.h"
#include "item_widget.h"

namespace Ui {
class Comment;
}

class CommentWidget : public ItemWidget
{
    Q_OBJECT

public:
  CommentWidget(const File::Comment &comment, const QString &username, const QColor &color, bool showMenu = true);
  ~CommentWidget();

signals:
  void editComment(CommentWidget *widget);
  void deleteComment(CommentWidget *widget);

public slots:
  int getUserId() const;
  File::CommentIdentifier getCommentIdentifier() const;
  void setIcon(const QIcon &icon);
  QString getText() const;
  File::Comment getComment() const;

  void updateText(const QString &text);

private slots:
  void _menu(bool checked);

private:
  void setName(const QString &name);

  Ui::Comment *ui;

  QPushButton *_widgetIcon, *_widgetMenu;
  QLabel *_widgetName, *_widgetDate;
  QPlainTextEdit *_widgetText;

  File::Comment _comment;
  QString _username;
  QIcon _icon;
  QColor _color;
};
