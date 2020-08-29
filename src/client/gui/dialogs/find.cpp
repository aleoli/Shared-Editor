#include "find.h"

#include <utility>
#include "../ui/ui_dialog_find.h"

#include <QTextDocument>

#include "exceptions.h"

using namespace se_exceptions;

Find::Find(QWidget *parent, QTextEdit *textEdit) :
  QDialog(parent),
  ui(new Ui::Find),
  _textEdit(textEdit)
{
  ui->setupUi(this);

  _widgetNext = ui->btn_next;
  _widgetCancel = ui->btn_close;
  _widgetSearch = ui->btn_search;
  _widgetBackwards = ui->cb_backwards;
  _widgetCase = ui->cb_caseSensitive;
  _widgetWhole = ui->cb_wholeWords;
  _widgetText = ui->ledit_search;
  _widgetMatches = ui->label_matches;

  connect(_widgetSearch, &QPushButton::clicked, this, &Find::_search);
  connect(_widgetNext, &QPushButton::clicked, this, &Find::_search);
  connect(_widgetCancel, &QPushButton::clicked, this, &Find::_cancel);
}

Find::~Find()
{
  delete ui;
}

void Find::clear() {
  _widgetMatches->setText("Search in text");
  _widgetText->clear();
  _widgetBackwards->setCheckState(Qt::Unchecked);
  _widgetCase->setCheckState(Qt::Unchecked);
  _widgetWhole->setCheckState(Qt::Unchecked);

  _widgetText->setFocus();
}

void Find::_search(bool checked) {
  auto backwards = _widgetBackwards->checkState() == Qt::Checked ? QTextDocument::FindBackward : QTextDocument::FindFlags();
  auto caseSensitive = _widgetCase->checkState() == Qt::Checked ? QTextDocument::FindCaseSensitively : QTextDocument::FindFlags();
  auto wholeWords = _widgetWhole->checkState() == Qt::Checked ? QTextDocument::FindWholeWords : QTextDocument::FindFlags();

  if(!_textEdit->find(_widgetText->text(), backwards | caseSensitive | wholeWords)) {

    //try going to the beginning/end
    auto cursor = QTextCursor(_textEdit->document());
    if(backwards) cursor.movePosition(QTextCursor::End);
    _textEdit->setTextCursor(cursor);

    if(!_textEdit->find(_widgetText->text(), backwards | caseSensitive | wholeWords)) {
      _widgetMatches->setText("No matches found");
      return;
    }
  }

  _widgetMatches->setText("Match found");
}

void Find::_cancel(bool checked) {
  reject();
}
