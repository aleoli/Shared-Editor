#include "texteditor.h"
#include "../ui/ui_texteditor.h"

#include "utils.h"
#include "exceptions.h"
#include "image_utils.h"

#include <QColorDialog>
#include <QFileDialog>
#include <QPrinter>
#include <QPrintDialog>
#include <QMenu>

#include <algorithm>

using namespace se_exceptions;

TextEditor::TextEditor(QWidget *parent) :
    MainWindow(parent),
    ui(new Ui::TextEditor),
    _highlighted(false),
    _blockSignals(false),
    _me(nullptr),
    _file(nullptr),
    _cursorPosition(0)
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
  _actionColor = ui->actionChange_text_color;
  _actionRename = ui->actionRename;
  _actionDeleteFile = ui->actionDelete;
  _actionInfo = ui->actionDetails;
  _actionDeleteText = ui->actionDelete_selected;
  _actionSelectAll = ui->actionSelect_all;
  _actionFind = ui->actionFind;

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
  reloadFile();
  updateActions();
  _gen.reset();
  reloadUsers();
  reloadComments();
  _setFilename();
  _textEdit->setFocus();
}

void TextEditor::setIcon() {
  auto icon = _user->getIcon();
  _menuOptions->setIcon(icon);

  if(_me) //should always happen
    _me->setIcon(icon);
}

void TextEditor::initOptionsWidget() {
  _menuOptions = new OptionsWidget;

  _menuBar->setCornerWidget(_menuOptions);

  connect(_menuOptions, &OptionsWidget::share, this, &TextEditor::_share);
  connect(_menuOptions, &OptionsWidget::comments, this, &TextEditor::_showComments);
  connect(_menuOptions, &OptionsWidget::connected, this, &TextEditor::_showConnected);
  connect(_menuOptions, &OptionsWidget::user, this, &TextEditor::_account);
  connect(_menuOptions, &OptionsWidget::rename, this, &TextEditor::_rename);
  connect(_user.get(), &User::fileNameChanged, this, &TextEditor::_setFilename);
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
  connect(_actionColor, &QAction::triggered, this, &TextEditor::_color);
  connect(_actionRename, &QAction::triggered, this, &TextEditor::_renameAction);
  connect(_actionDeleteFile, &QAction::triggered, this, &TextEditor::_deleteFile);
  connect(_actionInfo, &QAction::triggered, this, &TextEditor::_info);
  connect(_actionDeleteText, &QAction::triggered, this, &TextEditor::_deleteText);
  connect(_actionSelectAll, &QAction::triggered, this, &TextEditor::_selectAll);
  connect(_actionFind, &QAction::triggered, this, &TextEditor::_find);

  connect(_widgetFont, &QFontComboBox::currentFontChanged, this, &TextEditor::_font);
  connect(_widgetSize, QOverload<int>::of(&QComboBox::activated), this, &TextEditor::_size);

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

  _listOnline = new QListWidget(_dockOnline);
  _listOnline->setStyleSheet("QListWidget{background-color: #4E596F;}");
  _dockOnline->setWidget(_listOnline);

  _listOffline = new QListWidget(_dockOffline);
  _listOffline->setStyleSheet("QListWidget{background-color: #4E596F;}");
  _dockOffline->setWidget(_listOffline);

  _listComments = new QListWidget(_dockComments);
  _listComments->setStyleSheet("QListWidget{background-color: #4E596F;}");
  _dockComments->setWidget(_listComments);
}

void TextEditor::initTextEdit() {
  _textEdit = new TextEdit{this};
  setCentralWidget(_textEdit);
  _defColor = _textEdit->currentCharFormat().background();

  _dialogFind = new Find(this, _textEdit);

  connect(_textEdit->document(), &QTextDocument::contentsChange, this, &TextEditor::_contentsChange);
  connect(_textEdit, &QTextEdit::cursorPositionChanged, this, &TextEditor::_cursorChanged);
  connect(_textEdit, &QTextEdit::textChanged, this, &TextEditor::_updateCursors);
  connect(_textEdit, &TextEdit::resized, this, &TextEditor::_updateCursors);
  connect(_textEdit, &TextEdit::scrolled, this, &TextEditor::_updateCursors);
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

  setBorderColor(_widgetColor, fmt.foreground(), false);
  setBorderColor(_widgetMark, fmt.background(), true);

  _blockSignals = false;
}

void TextEditor::setBorderColor(QPushButton *button, const QBrush &brush, bool isBackground) {
  auto color = brush.color();

  //FIX
  if(color.alpha() == 255 && !brush.isOpaque()) {
    if(isBackground) color = QColor("white");
    else color = QColor("black");
  }
  else if(color.alpha() == 0) color = QColor("white");

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
    throw TextEditorException{"No horizontal alignment!"};
  }
}

void TextEditor::reloadFile() {
  _blockSignals = true;

  _textEdit->clear();
  _file = _user->getFile();
  refresh(true);

  _blockSignals = false;
}

void TextEditor::reloadComments() {
  _listComments->clear();
  _comments.clear();

  std::vector<File::Comment> comments;

  for(auto &comment : _file->getComments()) {
    comments.push_back(comment.second);
  }
  std::sort(comments.begin(), comments.end(),
    [](const File::Comment &i, const File::Comment &j){return i.creationDate < j.creationDate;});

  for(auto &comment : comments) {
    loadComment(comment.identifier.getUserId(), comment);
  }
}

void TextEditor::reloadUsers() {
  _users.clear(); //Before the others! Otherwise possible segmentation fault due to cursor deleted
  _listOnline->clear();
  _listOffline->clear();

  //add "me"
  _me = new RemoteUser(_user->getUserId(), _user->getUsername(), QColor("black"));
  _me->setIcon(_user->getIcon());
  _me->insert(0, _listOnline);

  for(auto &user : _file->getUsers()) {
    if (user.second.userId == _user->getUserId()) continue;
    addRemoteUser(user.second.userId, user.second.username, user.second.online);
    emit getUserIcon(user.second.userId);
  }
}

void TextEditor::addRemoteUser(int userId, const QString &username, bool online) {
  auto color = _gen.getColor();
  auto user = new RemoteUser(userId, username, color, new Cursor(_textEdit, color));
  _users[userId] = user;

  if(online)
    user->add(_listOnline);
  else
    user->add(_listOffline);
}

void TextEditor::setRemoteUserOnline(int userId) {
  // already checked presence in map
  auto user = _users[userId];
  user->setOnline(true);
  _users[userId] = RemoteUser::moveRemoteUser(user, _listOnline);

  _file->setOnline(userId, true);
}

void TextEditor::refresh(bool changeFile) {
  _blockSignals = true;

  //salvo pos del cursore mio
  int pos;
  if(!changeFile) pos = _textEdit->textCursor().position();

  //cancello
  _textEdit->clear();

  //refresh
  QTextCursor cursor{_textEdit->document()};
  QTextCharFormat fmt;
  QString text = "";
  _file->forEachSymbol([&cursor, &fmt, &text, this](const Symbol &sym){
    auto format = sym.getFormat();

    if(_highlighted) {
      auto userId = sym.getSymbolId().getUserId();
      format.setBackground(getUserColorHighlight(userId));
    }

    if(format != fmt) {
      cursor.setCharFormat(fmt);
      cursor.insertText(text);
      text.clear();
      fmt = format;
    }

    text.append(sym.getChar());
  });

  if(!text.isEmpty()) {
    cursor.setCharFormat(fmt);
    cursor.insertText(text);
  }

  //ripristino posizione
  if(!changeFile) {
    cursor.setPosition(pos);
    _textEdit->setTextCursor(cursor);
    _cursorPosition = pos;
  }

  _blockSignals = false;
}

std::pair<SymbolId, int> TextEditor::saveCursorPosition(const QTextCursor &cursor) {
  int position = cursor.position();
  SymbolId id;

  if(position == 0) {
    id = {-1,-1}; //TODO magari si può fare meglio
  }
  else {
    //TODO potenziale EXCEPTION, rimuovi try/catch e gestisci
    try {
      //debug("Pos: " + QString::number(position));
      id = _file->symbolAt(position-1).getSymbolId();
    }
    catch(...) {
      throw TextEditorException{"TextEditor::saveCursorPosition failed"};
    }
  }

  return {id, position};
}

int TextEditor::getCursorPosition(const SymbolId &id, int position) {
  int pos;

  try {
    pos = _file->symbolById(id).first + 1;
  }
  catch(FileSymbolsException& e) {
    pos = position;
  }

  return pos;
}

QColor TextEditor::getUserColor(int userId) {
  if(_me->getUserId() == userId) {
    return _me->getColor();
  }

  if(_users.count(userId) == 0) {
    throw TextEditorException{"TextEditor::getUserColor: user not present!"};
  }

  return _users[userId]->getColor();
}

QColor TextEditor::getUserColorHighlight(int userId) {
  if(_me->getUserId() == userId) {
    return QColor("white");
  }

  if(_users.count(userId) == 0) {
    throw TextEditorException{"TextEditor::getUserColorHighlight: user not present!"};
  }

  auto color = _users[userId]->getColor();
  color.setAlpha(64);
  return color;
}

QIcon TextEditor::getUserLoadedIcon(int userId) {
  if(_user->getUserId() == userId) {
    return _user->getIcon();
  }

  if(_users.count(userId) == 0) {
    throw TextEditorException{"TextEditor::getUserLoadedIcon: user not present!"};
  }

  return _users[userId]->getIcon();
}

/* ### SLOTS server->editor ### */
void TextEditor::userConnected(int fileId, int userId, const QString &username) {
  debug("TextEditor::userConnected");

  if(_users.count(userId) == 0) {
    addRemoteUser(userId, username);
    _file->addUser(userId, username);
  }
  else {
    setRemoteUserOnline(userId);
  }

  emit getUserIcon(userId);
}

void TextEditor::userDisconnected(int fileId, int userId) {
  debug("TextEditor::userDisconnected");

  if(_users.count(userId) == 0) {
    throw TextEditorException{"TextEditor::userDisconnected: user not present!?"};
  }

  auto user = _users[userId];
  user->setOnline(false);

  _users[userId] = RemoteUser::moveRemoteUser(user, _listOffline);
  //TODO reset cursor to first position

  _file->setOnline(userId, false);
}

void TextEditor::setUserIcon(int userId, const QString &icon) {
  debug("TextEditor::setUserIcon");

  if(_users.count(userId) == 0) {
    throw TextEditorException{"TextEditor::setUserIcon: user not present!?"};
  }

  auto user = _users[userId];
  auto decoded = image_utils::decodeImage(icon);
  user->setIcon(decoded);

  for(auto comment : _comments) {
    if(comment.first.getUserId() == userId) comment.second->setIcon(decoded);
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
  QAction *actionHome = new QAction{QIcon(":res/home.png"), "Home"};
  QAction *actionLogout = new QAction{QIcon(":res/logout.png"), "Logout"};

  QMenu menu;
  menu.addAction(actionHome);
  menu.addAction(actionLogout);
  auto action = menu.exec(QCursor::pos());

  if(!action) return;

  _dialogFind->close();
  if(action == actionHome) emit close(_user->getToken(), _user->getFileId());
  else emit logout(_user->getToken());
}

void TextEditor::_rename(const QString &name) {
  if(_user->setFileName(name)) // true if name has changed
    emit edit(_user->getToken(), _user->getFileId(), std::optional<QString>(name));

  _textEdit->setFocus();
}

void TextEditor::_setFilename() {
  _menuOptions->setFileName(_user->getFileName());
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

  setBorderColor(_widgetMark, col, true);
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

  setBorderColor(_widgetColor, col, false);
  auto fmt = _textEdit->currentCharFormat();
  fmt.setForeground(col);
  _textEdit->mergeCurrentCharFormat(fmt);
  _textEdit->setFocus();
}

void TextEditor::_download(bool checked) {
  debug("TextEditor::_download");

  auto filename = QFileDialog::getSaveFileName(this, "Save to PDF", QString(), "PDF files(*.pdf)");
  if(filename.isEmpty()) {
    return;
  }

  QPrinter printer(QPrinter::HighResolution);
  printer.setOutputFormat(QPrinter::PdfFormat);
  printer.setOutputFileName(filename);
  _textEdit->print(&printer);

  emit alert(Alert::INFO, "", "PDF saved");
}

void TextEditor::_print(bool checked) {
  debug("TextEditor::_print");

  QPrinter printer(QPrinter::HighResolution);
  QPrintDialog *dlg = new QPrintDialog(&printer, this);
  dlg->setWindowTitle(tr("Print Document"));

  if (dlg->exec() == QDialog::Accepted) {
    _textEdit->print(&printer);
    emit alert(Alert::INFO, "", "Printing requested successfully");
  }

  delete dlg;
}

void TextEditor::_close(bool checked) {
  debug("TextEditor::_close");
  _dialogFind->close();
  emit close(_user->getToken(), _user->getFileId());
}

void TextEditor::_buttonAlignment(QAbstractButton *button) {
  if(button == _widgetAlignL) _alignL();
  else if(button == _widgetAlignC) _alignC();
  else if(button == _widgetAlignR) _alignR();
  else if(button == _widgetJustify) _justify();
  else throw TextEditorException{"Alignment not valid!"};
}

void TextEditor::_actionAlignment(QAction *action) {
  if(action == _actionAlignL) _alignL();
  else if(action == _actionAlignC) _alignC();
  else if(action == _actionAlignR) _alignR();
  else if(action == _actionJustify) _justify();
  else throw TextEditorException{"Alignment not valid!"};
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

  if(checked) {
    _showConnected(true);
  }
  else {
    _dockOffline->setVisible(false);
  }

  //refresh & trigger background color button to change TODO change only that button
  refresh();
  updateActions();
}

void TextEditor::_renameAction(bool checked) {
  debug("TextEditor::_renameAction");
  _menuOptions->focusRename();
}

void TextEditor::_deleteFile(bool checked) {
  debug("TextEditor::_deleteFile");
  _dialogFind->close();
  emit remove(_user->getToken(), _user->getFileId());
}

void TextEditor::_info(bool checked) {
  debug("TextEditor::_info");
  refresh();
}

void TextEditor::_deleteText(bool checked) {
  debug("TextEditor::_deleteText");
  _textEdit->textCursor().removeSelectedText();
}

void TextEditor::_selectAll(bool checked) {
  debug("TextEditor::_selectAll");
  _textEdit->selectAll();
}

void TextEditor::_find(bool checked) {
  debug("TextEditor::_find");
  _dialogFind->clear();
  _dialogFind->show();
}
