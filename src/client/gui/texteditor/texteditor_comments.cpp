#include "texteditor.h"

#include "utils.h"
#include "exceptions.h"
#include "input.h"

using namespace se_exceptions;

CommentWidget *TextEditor::loadComment(int userId, const File::Comment &comment) {
  bool showMenu = userId == _user->getUserId();
  auto widget = new CommentWidget(comment, _file->getUsername(userId), getUserColor(userId), showMenu);

  if(showMenu) {
    connect(widget, &CommentWidget::editComment, this, &TextEditor::_editComment);
    connect(widget, &CommentWidget::deleteComment, this, &TextEditor::_deleteComment);
  }

  widget->insert(0, _listComments);
  _comments[comment.identifier] = widget;
  return widget;
}

CommentWidget *TextEditor::getComment(const File::Comment &comment) {
  if(_comments.count(comment.identifier) == 0) {
    throw GuiException{"TextEditor::getComment: comment not found"};
  }

  return _comments[comment.identifier];
}

void TextEditor::deleteComment(CommentWidget *widget) {
  widget->remove();
  _comments.erase(widget->getCommentIdentifier());
  delete widget->getItem();
  delete widget;
}

void TextEditor::commentRemoteInsert(int fileId, int userId, const File::Comment &comment) {
  auto widget = loadComment(userId, comment);
  widget->setIcon(getUserLoadedIcon(userId));
}

void TextEditor::commentRemoteUpdate(int fileId, int userId, const File::Comment &comment) {
  auto widget = getComment(comment);
  widget->updateText(comment.text);
}

void TextEditor::commentRemoteDelete(int fileId, int userId, const File::Comment &comment) {
  auto widget = getComment(comment);
  deleteComment(widget);
}

void TextEditor::_insertComment(bool checked) {
  debug("TextEditor::_insertComment");

  auto text = Input::show(this, "New comment");
  if(!text) return;

  auto comment = File::Comment{{_user->getCommentId(), _user->getUserId()}, *text, QDateTime::currentDateTime()};
  auto widget = loadComment(_user->getUserId(), comment);
  widget->setIcon(_user->getIcon());

  emit commentLocalInsert(_user->getToken(), _user->getFileId(), comment);
}

void TextEditor::_editComment(CommentWidget *widget) {
  debug("TextEditor::_editComment");

  auto text = Input::show(this, "Edit comment", widget->getText());
  if(!text) return;

  widget->updateText(*text);
  emit commentLocalUpdate(_user->getToken(), _user->getFileId(), widget->getComment());
}

void TextEditor::_deleteComment(CommentWidget *widget) {
  debug("TextEditor::_deleteComment");

  //TODO confirm dialog?

  auto comment = widget->getComment();
  deleteComment(widget);
  emit commentLocalDelete(_user->getToken(), _user->getFileId(), comment);
}
