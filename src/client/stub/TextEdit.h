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

signals:
  void closeFileQuery(int fileId);

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
  int _charId = 0;
  int _userId = 0;

  bool _flagBold, _flagCursor, _flagChange;
};
