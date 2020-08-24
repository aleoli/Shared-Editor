#include "texteditor.h"
#include "../ui/ui_texteditor.h"

#include "utils.h"
#include "exceptions.h"

#include <QColorDialog>

using namespace se_exceptions;

TextEditor::TextEditor(QWidget *parent) :
    MainWindow(parent),
    ui(new Ui::TextEditor),
    _highlighted(false),
    _blockSignals(false)
{
  ui->setupUi(this);

  _menuBar = ui->menubar;
  _dockOnline = ui->dock_connectedUsers;
  _dockOffline = ui->dock_disconnectedUsers;
  _dockComments = ui->dock_comments;
  _widgetUndo = ui->btn_undo;
  _widgetRedo = ui->btn_redo;
  _widgetCut = ui->btn_cut;
  _widgetCopy = ui->btn_copy;
  _widgetPaste = ui->btn_paste;
  _widgetBold = ui->btn_bold;
  _widgetItalics = ui->btn_italics;
  _widgetStrike = ui->btn_strike;
  _widgetUnderline = ui->btn_underline;
  _widgetMark = ui->btn_mark;
  _widgetColor = ui->btn_color;
  _widgetInsertComment = ui->btn_comment;
  _widgetDownload = ui->btn_download;
  _widgetPrint = ui->btn_print;
  _widgetAlignL = ui->btn_alignL;
  _widgetAlignC = ui->btn_alignC;
  _widgetAlignR = ui->btn_alignR;
  _widgetJustify = ui->btn_justify;
  _widgetHighlight = ui->btn_viewUsersTxt;

  _widgetFont = ui->fontComboBox;
  _widgetSize = ui->comboBox;

  _actionUndo = ui->actionUndo;
  _actionRedo = ui->actionRedo;
  _actionCut = ui->actionCut;
  _actionCopy = ui->actionCopy;
  _actionPaste = ui->actionPaste;
  _actionBold = ui->actionBold;
  _actionItalics = ui->actionItalics;
  _actionStrike = ui->actionStrike;
  _actionUnderline = ui->actionUnderline;
  _actionMark = ui->actionHighlight;
  _actionInsertComment = ui->actionComment;
  _actionDownload = ui->actionDownload;
  _actionPrint = ui->actionPrint;
  _actionShare = ui->actionShare;
  _actionShowComments = ui->actionComments;
  _actionShowConnectedUsers = ui->actionConnected_users;
  _actionClose = ui->actionClose;
  _actionAlignL = ui->actionAlign_left;
  _actionAlignC = ui->actionAlign_center;
  _actionAlignR = ui->actionAlign_right;
  _actionJustify = ui->actionJustify;
  _actionHighlight = ui->actionHighlight_users_text;

  initTextEdit();
  initOptionsWidget();
  initToolbarActions();
  initDocks();
}

TextEditor::~TextEditor()
{
    delete ui;
}

void TextEditor::clear() {
  // TODO Cose da fare quando la finestra è portata in primo piano (es. svuotare i campi)
  //TODO reset text editor!!
  _menuOptions->setFileName(_user->getFileName());
  _textEdit->setFocus();
}

void TextEditor::initOptionsWidget() {
  _menuOptions = new OptionsWidget;

  _menuBar->setCornerWidget(_menuOptions);

  connect(_menuOptions, &OptionsWidget::share, this, &TextEditor::_share);
  connect(_menuOptions, &OptionsWidget::comments, this, &TextEditor::_showComments);
  connect(_menuOptions, &OptionsWidget::connected, this, &TextEditor::_showConnected);
  connect(_menuOptions, &OptionsWidget::user, this, &TextEditor::_account);
  connect(_menuOptions, &OptionsWidget::rename, this, &TextEditor::_rename);
}

void TextEditor::initToolbarActions() {
  connect(_widgetUndo, &QPushButton::clicked, _textEdit, &TextEdit::undo);
  connect(_widgetRedo, &QPushButton::clicked, _textEdit, &TextEdit::redo);
  connect(_widgetCut, &QPushButton::clicked, _textEdit, &TextEdit::cut);
  connect(_widgetCopy, &QPushButton::clicked, _textEdit, &TextEdit::copy);
  connect(_widgetPaste, &QPushButton::clicked, _textEdit, &TextEdit::paste);
  connect(_widgetBold, &QPushButton::clicked, this, &TextEditor::_bold);
  connect(_widgetItalics, &QPushButton::clicked, this, &TextEditor::_italics);
  connect(_widgetStrike, &QPushButton::clicked, this, &TextEditor::_strike);
  connect(_widgetUnderline, &QPushButton::clicked, this, &TextEditor::_underline);
  connect(_widgetMark, &QPushButton::clicked, this, &TextEditor::_mark);
  connect(_widgetColor, &QPushButton::clicked, this, &TextEditor::_color);
  connect(_widgetInsertComment, &QPushButton::clicked, this, &TextEditor::_insertComment);
  connect(_widgetDownload, &QPushButton::clicked, this, &TextEditor::_download);
  connect(_widgetPrint, &QPushButton::clicked, this, &TextEditor::_print);
  connect(_widgetHighlight, &QPushButton::clicked, this, &TextEditor::_highlight);

  connect(_actionUndo, &QAction::triggered, _textEdit, &TextEdit::undo);
  connect(_actionRedo, &QAction::triggered, _textEdit, &TextEdit::redo);
  connect(_actionCut, &QAction::triggered, _textEdit, &TextEdit::cut);
  connect(_actionCopy, &QAction::triggered, _textEdit, &TextEdit::copy);
  connect(_actionPaste, &QAction::triggered, _textEdit, &TextEdit::paste);
  connect(_actionBold, &QAction::triggered, this, &TextEditor::_bold);
  connect(_actionItalics, &QAction::triggered, this, &TextEditor::_italics);
  connect(_actionStrike, &QAction::triggered, this, &TextEditor::_strike);
  connect(_actionUnderline, &QAction::triggered, this, &TextEditor::_underline);
  connect(_actionMark, &QAction::triggered, this, &TextEditor::_mark);
  connect(_actionInsertComment, &QAction::triggered, this, &TextEditor::_insertComment);
  connect(_actionDownload, &QAction::triggered, this, &TextEditor::_download);
  connect(_actionPrint, &QAction::triggered, this, &TextEditor::_print);
  connect(_actionShare, &QAction::triggered, this, &TextEditor::_share);
  connect(_actionShowComments, &QAction::triggered, this, &TextEditor::_showComments);
  connect(_actionShowConnectedUsers, &QAction::triggered, this, &TextEditor::_showConnected);
  connect(_actionClose, &QAction::triggered, this, &TextEditor::_close);
  connect(_actionHighlight, &QAction::triggered, this, &TextEditor::_highlight);

  connect(_widgetFont, &QFontComboBox::currentFontChanged, this, &TextEditor::_font);
  connect(_widgetSize, QOverload<int>::of(&QComboBox::activated), this, &TextEditor::_size);

  _actionUndo->setShortcut(QKeySequence::Undo);
  _actionRedo->setShortcut(QKeySequence::Redo);
  _actionCut->setShortcut(QKeySequence::Cut);
  _actionCopy->setShortcut(QKeySequence::Copy);
  _actionPaste->setShortcut(QKeySequence::Paste);
  _actionBold->setShortcut(QKeySequence::Bold);
  _actionItalics->setShortcut(QKeySequence::Italic);
  _actionUnderline->setShortcut(QKeySequence::Underline);
  _actionStrike->setShortcut(Qt::CTRL + Qt::Key_K);
  //TODO select all, print, close, find,

  setAlignmentGroups();

  // combobox for text size
  const QList<int> standardSizes = QFontDatabase::standardSizes();
  for (int size : standardSizes)
      _widgetSize->addItem(QString::number(size));
  _widgetSize->setCurrentIndex(standardSizes.indexOf(QApplication::font().pointSize()));
}

void TextEditor::initDocks() {
  _dockOnline->setFeatures(QDockWidget::NoDockWidgetFeatures);
  _dockOffline->setFeatures(QDockWidget::NoDockWidgetFeatures);
  _dockComments->setFeatures(QDockWidget::NoDockWidgetFeatures);

  _dockOffline->setVisible(false);
}

void TextEditor::initTextEdit() {
  _textEdit = new TextEdit;
  setCentralWidget(_textEdit);
  _defColor = _textEdit->currentCharFormat().background();
}

void TextEditor::setAlignmentGroups() {
  _alignmentButtons = new QButtonGroup(this);
  _alignmentActions = new QActionGroup(this);

  _alignmentButtons->addButton(_widgetAlignL);
  _alignmentButtons->addButton(_widgetAlignC);
  _alignmentButtons->addButton(_widgetAlignR);
  _alignmentButtons->addButton(_widgetJustify);

  _alignmentActions->addAction(_actionAlignL);
  _alignmentActions->addAction(_actionAlignC);
  _alignmentActions->addAction(_actionAlignR);
  _alignmentActions->addAction(_actionJustify);

  connect(_alignmentButtons, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked), this, &TextEditor::_buttonAlignment);
  connect(_alignmentActions, &QActionGroup::triggered, this, &TextEditor::_actionAlignment);
}

void TextEditor::updateActions() {
  _blockSignals = true;

  auto fmt = _textEdit->currentCharFormat();
  _widgetBold->setChecked(fmt.font().bold());
  _actionBold->setChecked(fmt.font().bold());
  _widgetItalics->setChecked(fmt.fontItalic());
  _actionItalics->setChecked(fmt.fontItalic());
  _widgetStrike->setChecked(fmt.fontStrikeOut());
  _actionStrike->setChecked(fmt.fontStrikeOut());
  _widgetUnderline->setChecked(fmt.fontUnderline());
  _actionUnderline->setChecked(fmt.fontUnderline());
  _widgetFont->setCurrentText(fmt.font().family());
  _widgetSize->setCurrentIndex(QFontDatabase::standardSizes().indexOf(fmt.font().pointSize()));

  updateAlignment(_textEdit->alignment());

  setBorderColor(_widgetColor, fmt.foreground().color());

  // TODO: FIX THIS!!
  // temp: dato che normalmente il backgroundColor ritornato da sto metodo è nero (a meno che non lo si cambia)
  // non posso mostrare quel colore nel pulsante, quindi mostro bianco (temporaneamente!!!)
  auto backgroundColor = fmt.background().color();
  if(backgroundColor == QColor::fromRgb(0,0,0) || backgroundColor == QColor::fromRgb(0,0,0,0))
    setBorderColor(_widgetMark, QColor::fromRgb(255,255,255));
  else
    setBorderColor(_widgetMark, backgroundColor);

  _blockSignals = false;
}

void TextEditor::setBorderColor(QPushButton *button, const QColor &color) {
  auto stylesheet = button->styleSheet();
  stylesheet.append("QPushButton {border-color: " + color.name(QColor::HexArgb) + ";}");
  button->setStyleSheet(stylesheet);
}

void TextEditor::updateAlignment(Qt::Alignment al) {
  if(al & Qt::AlignLeft) {
    _widgetAlignL->setChecked(true);
    _actionAlignL->setChecked(true);
  }
  else if(al & Qt::AlignRight) {
    _widgetAlignR->setChecked(true);
    _actionAlignR->setChecked(true);
  }
  else if(al & Qt::AlignHCenter) {
    _widgetAlignC->setChecked(true);
    _actionAlignC->setChecked(true);
  }
  else if(al & Qt::AlignJustify) {
    _widgetJustify->setChecked(true);
    _actionJustify->setChecked(true);
  }
  else {
    throw GuiException{"No horizontal alignment!"};
  }
}

/* ### SLOTS buttons/actions ### */

void TextEditor::_share() {
  emit share(_user->getToken(), _user->getFileId());
}

void TextEditor::_showComments(bool checked) {
  _actionShowComments->setChecked(checked);
  _menuOptions->setShowCommentsChecked(checked);
  _dockComments->setVisible(checked);
}

void TextEditor::_showConnected(bool checked) {
  _actionShowConnectedUsers->setChecked(checked);
  _menuOptions->setShowConnectedChecked(checked);
  _dockOnline->setVisible(checked);

  if(_highlighted)
    _dockOffline->setVisible(checked);
  else
    _dockOffline->setVisible(false);
}

void TextEditor::_account() {
  //TODO show menu, logout or close (and maybe edit??)
  emit logout(_user->getToken());
  //emit close(_user->getToken(), _user->getFileId());
}

void TextEditor::_rename(const QString &name) {
  _user->setFileName(name);
  emit editFile(_user->getToken(), _user->getFileId(), std::optional<QString>(name));
}

void TextEditor::_bold(bool checked) {
  if(_blockSignals) return;
  debug("TextEditor::_bold");
  _widgetBold->setChecked(checked);
  _actionBold->setChecked(checked);

  auto fmt = _textEdit->currentCharFormat();
  fmt.setFontWeight(checked ? QFont::Bold : QFont::Normal);
  _textEdit->mergeCurrentCharFormat(fmt);
  _textEdit->setFocus();
}

void TextEditor::_italics(bool checked) {
  if(_blockSignals) return;
  debug("TextEditor::_italics");

  _widgetItalics->setChecked(checked);
  _actionItalics->setChecked(checked);

  auto fmt = _textEdit->currentCharFormat();
  fmt.setFontItalic(checked);
  _textEdit->mergeCurrentCharFormat(fmt);
  _textEdit->setFocus();
}

void TextEditor::_strike(bool checked) {
  if(_blockSignals) return;
  debug("TextEditor::_strike");

  _widgetStrike->setChecked(checked);
  _actionStrike->setChecked(checked);

  auto fmt = _textEdit->currentCharFormat();
  fmt.setFontStrikeOut(checked);
  _textEdit->mergeCurrentCharFormat(fmt);
  _textEdit->setFocus();
}

void TextEditor::_underline(bool checked) {
  if(_blockSignals) return;
  debug("TextEditor::_underline");

  _widgetUnderline->setChecked(checked);
  _actionUnderline->setChecked(checked);

  auto fmt = _textEdit->currentCharFormat();
  fmt.setFontUnderline(checked);
  _textEdit->mergeCurrentCharFormat(fmt);
  _textEdit->setFocus();
}

void TextEditor::_mark(bool checked) {
  if(_blockSignals) return;
  debug("TextEditor::_mark");

  if(_highlighted) {
    emit alert(Alert::ERROR, HIGHLIGHT_ON);
    return;
  }

  QColor col = QColorDialog::getColor(_textEdit->textBackgroundColor(), this, "Select color", QColorDialog::ShowAlphaChannel);
  if (!col.isValid()) {
    return;
  }

  setBorderColor(_widgetMark, col);
  auto fmt = _textEdit->currentCharFormat();
  fmt.setBackground(col);
  _textEdit->mergeCurrentCharFormat(fmt);
  _textEdit->setFocus();
}

void TextEditor::_color(bool checked) {
  if(_blockSignals) return;
  debug("TextEditor::_color");

  QColor col = QColorDialog::getColor(_textEdit->textColor(), this, "Select color", QColorDialog::ShowAlphaChannel);
  if (!col.isValid()) {
    return;
  }

  setBorderColor(_widgetColor, col);
  auto fmt = _textEdit->currentCharFormat();
  fmt.setForeground(col);
  _textEdit->mergeCurrentCharFormat(fmt);
  _textEdit->setFocus();
}

void TextEditor::_insertComment(bool checked) {
  debug("TextEditor::_insertComment");

  //TODO
}

void TextEditor::_download(bool checked) {
  debug("TextEditor::_download");

  //TODO
}

void TextEditor::_print(bool checked) {
  debug("TextEditor::_print");

  //TODO
}

void TextEditor::_close(bool checked) {
  debug("TextEditor::_close");
  emit close(_user->getToken(), _user->getFileId());
}

void TextEditor::_buttonAlignment(QAbstractButton *button) {
  if(button == _widgetAlignL) _alignL();
  else if(button == _widgetAlignC) _alignC();
  else if(button == _widgetAlignR) _alignR();
  else if(button == _widgetJustify) _justify();
  else throw GuiException{"Alignment not valid!"};
}

void TextEditor::_actionAlignment(QAction *action) {
  if(action == _actionAlignL) _alignL();
  else if(action == _actionAlignC) _alignC();
  else if(action == _actionAlignR) _alignR();
  else if(action == _actionJustify) _justify();
  else throw GuiException{"Alignment not valid!"};
}

void TextEditor::_alignL() {
  if(_blockSignals) return;
  debug("TextEditor::_alignL");
  _widgetAlignL->setChecked(true);
  _actionAlignL->setChecked(true);

  _textEdit->setAlignment(Qt::AlignLeft);
  _textEdit->setFocus();
}

void TextEditor::_alignC() {
  if(_blockSignals) return;
  debug("TextEditor::_alignC");
  _widgetAlignC->setChecked(true);
  _actionAlignC->setChecked(true);

  _textEdit->setAlignment(Qt::AlignHCenter);
  _textEdit->setFocus();
}

void TextEditor::_alignR() {
  if(_blockSignals) return;
  debug("TextEditor::_alignR");
  _widgetAlignR->setChecked(true);
  _actionAlignR->setChecked(true);

  _textEdit->setAlignment(Qt::AlignRight);
  _textEdit->setFocus();
}

void TextEditor::_justify() {
  if(_blockSignals) return;
  debug("TextEditor::_justify");
  _widgetJustify->setChecked(true);
  _actionJustify->setChecked(true);

  _textEdit->setAlignment(Qt::AlignJustify);
  _textEdit->setFocus();
}

void TextEditor::_font(const QFont &font) {
  if(_blockSignals) return;
  debug("TextEditor::_font");

  auto fmt = _textEdit->currentCharFormat();
  fmt.setFontFamily(font.family());
  _textEdit->mergeCurrentCharFormat(fmt);
  _textEdit->setFocus();
}

void TextEditor::_size(int index) {
  if(_blockSignals) return;
  bool ok;
  auto size = _widgetSize->itemText(index).toFloat(&ok);
  debug("TextEditor::_size " + QString::number(size));

  if(!ok) {
    emit alert(Alert::ERROR, INVALID_SIZE);
    return;
  }

  auto fmt = _textEdit->currentCharFormat();
  fmt.setFontPointSize(size);
  _textEdit->mergeCurrentCharFormat(fmt);
  _textEdit->setFocus();
}

void TextEditor::_highlight(bool checked) {
  debug("TextEditor::_highlight");
  _highlighted = checked;
  _widgetHighlight->setChecked(checked);
  _actionHighlight->setChecked(checked);

  if(checked)
    _showConnected(true);

  //TODO highlight on/off

  //trigger background color button to change TODO change only that button
  updateActions();
}
