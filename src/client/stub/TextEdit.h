#pragma once

#include <QMainWindow>
#include <QTextEdit>
#include <vector>
#include <QChar>
#include <QListWidget>
#include <QCloseEvent>
#include <QListWidgetItem>
#include <QString>
#include <unordered_map>

#include "File.h"
#include "Cursor.h"
#include "ColorGenerator.h"

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
      int userId;
      QListWidgetItem *item = nullptr;
      QString username;
      int charId;
      QColor color;
    } localUser;

public:
  explicit TextEdit(QWidget *parent = nullptr);
  void closeEvent (QCloseEvent *event) override;

  void setFile(const File &f, int charId = 0);
  void setUser(int userId, QString username);

public slots:
  // messaggi dal server
  void remoteInsertQuery(int fileId, std::vector<Symbol> symbols);
  void remoteDeleteQuery(int fileId, std::vector<SymbolId> ids);
  void remoteUpdateQuery(int fileId, std::vector<Symbol> symbols);
  void userConnectedQuery(int fileId, int clientId, QString username);
  void userDisconnectedQuery(int fileId, int clientId);
  void remoteMoveQuery(int fileId, int clientId, SymbolId symbolId, int cursorPosition);

signals:
  void closeFileQuery(int fileId);
  void localInsertQuery(int fileId, std::vector<Symbol> symbols);
  void localDeleteQuery(int fileId, std::vector<SymbolId> ids);
  void localUpdateQuery(int fileId, std::vector<Symbol> symbols);
  void localMoveQuery(int fileId, SymbolId symbolId, int cursorPosition);

private slots:
  void textBold();
  void addLetter();
  void printTextFile();
  void change(int pos, int removed, int added);
  void cursorChanged();

private:
  void setupFileActions();
  void setupEditActions();
  void setupTextActions();

  void initDock();
  void refresh();

  QTextEdit *_textEdit;
  QAction *_actionTextBold;
  QListWidget *_dock;

  File _file;
  localUser _user; // TODO si può fare un check su questo attributo per vedere se ci sono errori

  //struttura dati per cursori etc
  std::unordered_map<int, remoteUser> _users;

  bool _flagBold, _flagCursor, _flagChange;
  ColorGenerator _gen;
};
