#pragma once

#include <QMainWindow>
#include <vector>
#include <QChar>
#include <QListWidget>
#include <QCloseEvent>
#include <QListWidgetItem>
#include <QString>
#include <unordered_map>
#include <QFontComboBox>
#include <QComboBox>

#include "File.h"
#include "Cursor.h"
#include "ColorGenerator.h"
#include "QTextEditImpl.h"

class TextEdit : public QMainWindow {
    Q_OBJECT

    typedef struct {
      int userId;
      QListWidgetItem *item;
      QString username;
      Cursor *cursor;
      QColor color;
    } remoteUser;

    typedef struct {
      bool initialized = false;
      int userId;
      QListWidgetItem *item = nullptr;
      QString username;
      int charId;
    } localUser;

public:
  explicit TextEdit(QWidget *parent = nullptr);
  void closeEvent (QCloseEvent *event) override;

  void setFile(const File &f, int charId = 0);
  File& getFile();
  void setFileId(int id);
  int getFileId() const;
  void setUser(int userId, QString username);
  void showShareLink();
  void setConnectedUsers();

public slots:
  // messaggi dal server
  void remoteInsertQuery(int fileId, int clientId, std::vector<Symbol> symbols);
  void remoteDeleteQuery(int fileId, int clientId, std::vector<SymbolId> ids);
  void remoteUpdateQuery(int fileId, int clientId, std::vector<Symbol> symbols);
  void userConnectedQuery(int fileId, int clientId, QString username);
  void userDisconnectedQuery(int fileId, int clientId);
  void remoteMoveQuery(int fileId, int clientId, SymbolId symbolId, int cursorPosition);
  void setShareLink(QString shareLink);
  void updateCursors();

signals:
  void closeFileQuery(int fileId);
  void localInsertQuery(int fileId, std::vector<Symbol> symbols);
  void localDeleteQuery(int fileId, std::vector<SymbolId> ids);
  void localUpdateQuery(int fileId, std::vector<Symbol> symbols);
  void localMoveQuery(int fileId, SymbolId symbolId, int cursorPosition);
  void getLinkQuery(int fileId);

private slots:
  void textBold();
  void textItalic();
  void textUnderline();
  void textColor();
  void textBackgroundColor();
  void textFamily(const QString &f);
  void textSize(const QString &p);

  void addLetter();
  void printTextFile();
  void change(int pos, int removed, int added);
  void cursorChanged();
  void formatChanged();

private:
  void setupFileActions();
  void setupEditActions();
  void setupTextActions();
  void updateActions();

  void handleUpdate(int pos, int nchars);
  void handleDelete(int pos, int removed);
  void handleInsert(int pos, int added);

  void initDock();
  void refresh(bool changeFile = false);
  void clear();
  void reset();
  void share();

  std::pair<SymbolId, int> saveCursorPosition(const QTextCursor &cursor);
  int getCursorPosition(SymbolId id, int position);

  QTextEditImpl *_textEdit;

  QAction *_actionTextBold;
  QAction *_actionTextItalic;
  QAction *_actionTextUnderline;
  QAction *_actionTextColor;
  QAction *_actionTextBackgroundColor;
  QFontComboBox *_comboFont;
  QComboBox *_comboSize;

  QListWidget *_dock;

  File _file;
  std::optional<QString> _shareLink;
  int _fileId;
  localUser _user; // TODO si può fare un check su questo attributo per vedere se ci sono errori

  //struttura dati per cursori etc
  std::unordered_map<int, remoteUser> _users;

  //booleano che utilizzo per evitare che certi slot vengano emessi quando non voglio
  //esempio: faccio clear -> non voglio che venga eseguito lo slot change()
  bool _blockSignals;
  int _cursorPosition;
  ColorGenerator _gen;
};
