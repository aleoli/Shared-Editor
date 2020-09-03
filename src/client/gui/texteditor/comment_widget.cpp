#include "comment_widget.h"
#include "../ui/ui_texteditor_comment.h"

#include <QMenu>
#include <QAction>
#include <QCursor>

#include "utils.h"
#include "exceptions.h"
#include "image_utils.h"

using namespace se_exceptions;

CommentWidget::CommentWidget(const File::Comment &comment, const QString &username, const QColor &color, bool showMenu) :
  ItemWidget(),
  ui(new Ui::Comment),
  _comment(comment),
  _username(username),
  _color(color)
{
  ui->setupUi(this);

  _widgetIcon = ui->label_icon;
  _widgetName = ui->label_name;
  _widgetDate = ui->label_date;
  _widgetText = ui->text;
  _widgetMenu = ui->btn_options;

  _widgetName->setStyleSheet("QLabel {color : " + _color.name(QColor::HexArgb) + "; }");
  setName(_username);

  setIcon(image_utils::loadImage(DEFAULT_ICON));
  _widgetText->setPlainText(_comment.text);
  _widgetDate->setText(_comment.creationDate.toLocalTime().toString("dd.MM.yyyy hh:mm:ss"));

  _item->setSizeHint(this->size());

  if(showMenu)
    connect(_widgetMenu, &QPushButton::clicked, this, &CommentWidget::_menu);
  else
    _widgetMenu->setVisible(false);
}

CommentWidget::~CommentWidget()
{
    delete ui;
}

int CommentWidget::getUserId() const {
  return _comment.identifier.getFirst();
}

File::CommentIdentifier CommentWidget::getCommentIdentifier() const {
  return _comment.identifier;
}

void CommentWidget::setIcon(const QIcon &icon) {
  if(icon.isNull()) return;

  auto size = _widgetIcon->size();
  _widgetIcon->setIcon(icon);
  _widgetIcon->setIconSize(QSize(size.width()-2, size.height()-2));
  _icon = icon;
}

void CommentWidget::updateText(const QString &text) {
  _comment.text = text;
  _widgetText->setPlainText(text);
}

QString CommentWidget::getText() const {
  return _comment.text;
}

File::Comment CommentWidget::getComment() const {
  return _comment;
}

void CommentWidget::setName(const QString &name) {
  if(name.length() > MAX_SIZE_NAME) {
    auto substr = name.mid(0, MAX_SIZE_NAME);
    substr.append("...");
    _widgetName->setText(substr);
    _widgetName->setToolTip(name);
  }
  else {
    _widgetName->setText(name);
  }
}

void CommentWidget::_menu(bool checked) {
  QAction *actionEdit = new QAction{QIcon(":res/rename.png"), "Edit"};
  QAction *actionDelete = new QAction{QIcon(":res/delete.png"), "Delete"};

  QMenu menu;
  menu.addAction(actionEdit);
  menu.addAction(actionDelete);
  auto action = menu.exec(QCursor::pos());

  if(!action) return;

  if(action == actionEdit) emit editComment(this);
  else emit deleteComment(this);
}
