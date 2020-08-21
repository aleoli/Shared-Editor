#pragma once

#include <QMenuBar>
#include <QPushButton>
#include <QDockWidget>

#include "main_window.h"
#include "alert_messages.h"
#include "comment_widget.h"
#include "menu_options.h"
#include "user_widget.h"
#include "text_edit.h"

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
  void editFile(const QString &token, int fileId, const std::optional<QString> &name);

public slots:
  virtual void clear();

private:
  void initOptionsWidget();
  void initDocks();
  void initTextEdit();

  void _share();
  void _showComments(bool checked);
  void _showConnected(bool checked);
  void _account();
  void _rename(const QString &name);

  Ui::TextEditor *ui;

  QMenuBar *_menuBar;
  QDockWidget *_dockOnline, *_dockOffline, *_dockComments;

  OptionsWidget *_menuOptions;
  TextEdit *_textEdit;
};
