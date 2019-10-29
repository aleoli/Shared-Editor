#pragma once

#include <QMainWindow>
#include <QTextEdit>
#include <vector>
#include <QChar>
#include <QListWidget>
#include <QCloseEvent>

#include "File.h"

class TextEdit : public QMainWindow {
    Q_OBJECT
public:
  explicit TextEdit(QWidget *parent = nullptr);
  void closeEvent (QCloseEvent *event) override;

  void setFile(const File &f, int charId = 0);
  void setUserId(int userId);

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
  void addFakeUsers();

  QTextEdit *_textEdit;
  QAction *_actionTextBold;
  QListWidget *_dock;

  File _file;
  //TODO si possono fare check su questi attributi per vedere se c'è stato qualche errore
  int _charId = -1;
  int _userId = -1;

  bool _flagBold, _flagCursor, _flagChange;
};
