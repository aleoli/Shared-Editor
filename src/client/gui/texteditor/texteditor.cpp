#include "texteditor.h"
#include "../ui/ui_texteditor.h"

#include "utils.h"
#include "exceptions.h"

using namespace se_exceptions;

TextEditor::TextEditor(QWidget *parent) :
    MainWindow(parent),
    ui(new Ui::TextEditor)
{
    ui->setupUi(this);

    _menuBar = findChild<QMenuBar *>("menubar");
    _dockOnline = findChild<QDockWidget *>("dock_connectedUsers");
    _dockComments = findChild<QDockWidget *>("dock_comments");
    //TODO dock offline

    if(!_menuBar || !_dockOnline || !_dockComments) {
      throw GuiException{"One or more widgets in TextEditor are null"};
    }

    initOptionsWidget();
    initDocks();
    initTextEdit();
}

TextEditor::~TextEditor()
{
    delete ui;
}

void TextEditor::clear() {
  // TODO Cose da fare quando la finestra è portata in primo piano (es. svuotare i campi)
  _menuOptions->setFileName(_user->getFileName());
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

void TextEditor::initDocks() {
  _dockOnline->setFeatures(QDockWidget::NoDockWidgetFeatures);
  _dockComments->setFeatures(QDockWidget::NoDockWidgetFeatures);
}

void TextEditor::initTextEdit() {
  _textEdit = new TextEdit;
  setCentralWidget(_textEdit);
}

void TextEditor::_share() {
  emit share(_user->getToken(), _user->getFileId());
}

void TextEditor::_showComments(bool checked) {
  _dockComments->setVisible(checked);
}

void TextEditor::_showConnected(bool checked) {
  _dockOnline->setVisible(checked);
  //TODO also offline
}

void TextEditor::_account() {
  //TODO show menu, logout or close (and maybe edit??)
  //emit logout(_user->getToken());
  emit close(_user->getToken(), _user->getFileId());
}

void TextEditor::_rename(const QString &name) {
  _user->setFileName(name);
  emit editFile(_user->getToken(), _user->getFileId(), std::optional<QString>(name));
}
