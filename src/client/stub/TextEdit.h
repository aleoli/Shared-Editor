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
      bool initialized = false;
      int userId;
      QListWidgetItem *item = nullptr;
      QString username;
      int charId;
    } localUser;

public:
  explicit TextEdit(QWidget *parent = nullptr);
  void closeEvent (QCloseEvent *event) override;
  void showEvent(QShowEvent* event) override;

  void setFile(const File &f, int charId = 0);
  File& getFile();
  void setFileId(int id);
  int getFileId() const;
  void setUser(int userId, QString username);
  void showShareLink();

public slots:
  // messaggi dal server
  void remoteInsertQuery(int fileId, int clientId, std::vector<Symbol> symbols);
  void remoteDeleteQuery(int fileId, int clientId, std::vector<SymbolId> ids);
  void remoteUpdateQuery(int fileId, int clientId, std::vector<Symbol> symbols);
  void userConnectedQuery(int fileId, int clientId, QString username);
  void userDisconnectedQuery(int fileId, int clientId);
  void remoteMoveQuery(int fileId, int clientId, SymbolId symbolId, int cursorPosition);
  void setShareLink(QString shareLink);

signals:
  void closeFileQuery(int fileId);
  void localInsertQuery(int fileId, std::vector<Symbol> symbols);
  void localDeleteQuery(int fileId, std::vector<SymbolId> ids);
  void localUpdateQuery(int fileId, std::vector<Symbol> symbols);
  void localMoveQuery(int fileId, SymbolId symbolId, int cursorPosition);
  void getLinkQuery(int fileId);

private slots:
  void textBold();
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

  void initDock();
  void refresh(bool changeFile = false);
  void clear();
  void reset();
  void share();

  std::pair<SymbolId, int> saveCursorPosition(const QTextCursor &cursor);
  int getCursorPosition(SymbolId id, int position);

  QTextEdit *_textEdit;
  QAction *_actionTextBold;
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
