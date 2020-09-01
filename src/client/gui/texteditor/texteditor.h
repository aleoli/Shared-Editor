#pragma once

#include <QMenuBar>
#include <QPushButton>
#include <QDockWidget>
#include <QFontComboBox>
#include <QComboBox>
#include <QAction>
#include <QButtonGroup>
#include <QActionGroup>
#include <QFont>
#include <QColor>
#include <QBrush>
#include <QListWidget>
#include <unordered_map>
#include <list>

#include "main_window.h"
#include "alert_messages.h"
#include "comment_widget.h"
#include "menu_options.h"
#include "remote_user.h"
#include "text_edit.h"
#include "color_generator.h"
#include "cursor.h"
#include "File.h"
#include "Symbol.h"
#include "SymbolId.h"
#include "find.h"

namespace Ui {
class TextEditor;
}

class TextEditor : public MainWindow
{
    Q_OBJECT

public:
  explicit TextEditor(QWidget *parent = nullptr);
  ~TextEditor();

signals:
  void logout(const QString &token);
  void share(const QString &token, int fileId);
  void close(const QString &token, int fileId);
  void remove(const QString &token, int fileId);
  void edit(const QString &token, int fileId, const std::optional<QString> &name);
  void getUserIcon(int userId);

  // Messages to server
  void commentLocalInsert(const QString &token, int fileId, const File::Comment &comment);
  void commentLocalUpdate(const QString &token, int fileId, const File::Comment &comment);
  void commentLocalDelete(const QString &token, int fileId, const File::Comment &comment);

  void localInsert(const QString &token, int fileId, const std::list<Symbol> &symbols);
  void localDelete(const QString &token, int fileId, const std::vector<SymbolId> &ids);
  void localUpdate(const QString &token, int fileId, const std::vector<Symbol> &symbols);
  void localMove(const QString &token, int fileId, const SymbolId &symbolId, int cursorPosition);

public slots:
  virtual void clear();
  void setIcon();

  // Messages from server
  void userConnected(int fileId, int userId, const QString &username);
  void userDisconnected(int fileId, int userId);
  void setUserIcon(int userId, const QString &icon);

  void commentRemoteInsert(int fileId, int userId, const File::Comment &comment);
  void commentRemoteUpdate(int fileId, int userId, const File::Comment &comment);
  void commentRemoteDelete(int fileId, int userId, const File::Comment &comment);

  void remoteInsert(int fileId, int userId, const std::list<Symbol>& symbols);
  void remoteDelete(int fileId, int userId, const std::vector<SymbolId>& ids);
  void remoteUpdate(int fileId, int userId, const std::vector<Symbol>& symbols);
  void remoteMove(int fileId, int userId, const SymbolId &symbolId, int cursorPosition);

private slots:
  void _bold(bool checked);
  void _italics(bool checked);
  void _strike(bool checked);
  void _underline(bool checked);
  void _mark(bool checked);
  void _color(bool checked);
  void _download(bool checked);
  void _print(bool checked);
  void _share();
  void _showComments(bool checked);
  void _showConnected(bool checked);
  void _account();
  void _rename(const QString &name);
  void _close(bool checked);
  void _buttonAlignment(QAbstractButton *button);
  void _actionAlignment(QAction *action);
  void _alignL();
  void _alignC();
  void _alignR();
  void _justify();
  void _font(const QFont &font);
  void _size(int index);
  void _highlight(bool checked);
  void _renameAction(bool checked);
  void _deleteFile(bool checked);
  void _info(bool checked);
  void _deleteText(bool checked);
  void _selectAll(bool checked);
  void _find(bool checked);

  //comments
  void _insertComment(bool checked);
  void _editComment(CommentWidget *widget);
  void _deleteComment(CommentWidget *widget);

  //CRDT
  void _contentsChange(int pos, int removed, int added);
  bool _isFakeUpdate(int pos, int removed, int added, std::list<Symbol>::iterator it);
  void _handleDelete(int pos, int removed, std::list<Symbol>::iterator &it);
  void _handleInsert(int pos, int added, std::list<Symbol>::iterator &it);
  void _cursorChanged();
  void _updateCursors();
  void _partialRefresh(int pos, int added);

private:
  void initOptionsWidget();
  void initToolbarActions();
  void initDocks();
  void initTextEdit();
  void setAlignmentGroups();
  void updateActions();
  void setBorderColor(QPushButton *button, const QBrush &brush, bool isBackground);
  void updateAlignment(Qt::Alignment al);
  void reloadFile();
  void reloadComments();
  void reloadUsers();
  void addRemoteUser(int userId, const QString &username, bool online = true);
  void setRemoteUserOnline(int userId);
  void refresh(bool changeFile = false);
  std::pair<SymbolId, int> saveCursorPosition(const QTextCursor &cursor);
  int getCursorPosition(const SymbolId &id, int position);
  QColor getUserColor(int userId);
  QColor getUserColorHighlight(int userId);
  QIcon getUserLoadedIcon(int userId);

  CommentWidget *loadComment(int userId, const File::Comment &comment);
  CommentWidget *getComment(const File::Comment &comment);
  void deleteComment(CommentWidget *widget);

  Ui::TextEditor *ui;

  QMenuBar *_menuBar;
  QDockWidget *_dockOnline, *_dockOffline, *_dockComments;
  QListWidget *_listOnline, *_listOffline, *_listComments;

  //TODO widget highlight

  //Toolbar
  QPushButton *_widgetUndo, *_widgetRedo, *_widgetCut, *_widgetCopy, *_widgetPaste,
    *_widgetBold, *_widgetItalics, *_widgetStrike, *_widgetUnderline, *_widgetMark,
    *_widgetColor, *_widgetInsertComment, *_widgetDownload, *_widgetPrint,
    *_widgetAlignL, *_widgetAlignC, *_widgetAlignR, *_widgetJustify, *_widgetHighlight;

  QFontComboBox *_widgetFont;
  QComboBox *_widgetSize;

  //Menu
  QAction *_actionUndo, *_actionRedo, *_actionCut, *_actionCopy, *_actionPaste,
  *_actionBold, *_actionItalics, *_actionStrike, *_actionUnderline, *_actionMark,
  *_actionInsertComment, *_actionDownload, *_actionPrint, *_actionShare,
  *_actionShowComments, *_actionShowConnectedUsers, *_actionClose,
  *_actionAlignL, *_actionAlignC, *_actionAlignR, *_actionJustify, *_actionHighlight,
  *_actionColor, *_actionRename, *_actionDeleteFile, *_actionInfo, *_actionDeleteText,
  *_actionSelectAll, *_actionFind;
  //TODO other actions

  OptionsWidget *_menuOptions;
  TextEdit *_textEdit;

  QButtonGroup *_alignmentButtons;
  QActionGroup *_alignmentActions;

  bool _highlighted, _blockSignals;
  QBrush _defColor;
  std::unordered_map<int, RemoteUser*> _users;
  std::map<File::CommentIdentifier, CommentWidget*> _comments;
  RemoteUser *_me;
  File *_file;
  ColorGenerator _gen;
  int _cursorPosition;
  Find *_dialogFind;
};
