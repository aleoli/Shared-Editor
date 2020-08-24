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

private slots:
  void _bold(bool checked);
  void _italics(bool checked);
  void _strike(bool checked);
  void _underline(bool checked);
  void _mark(bool checked);
  void _color(bool checked);
  void _insertComment(bool checked);
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

private:
  void initOptionsWidget();
  void initToolbarActions();
  void initDocks();
  void initTextEdit();
  void setAlignmentGroups();
  void updateActions();
  void setBorderColor(QPushButton *button, const QColor &color);
  void updateAlignment(Qt::Alignment al);

  Ui::TextEditor *ui;

  QMenuBar *_menuBar;
  QDockWidget *_dockOnline, *_dockOffline, *_dockComments;

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
  *_actionAlignL, *_actionAlignC, *_actionAlignR, *_actionJustify, *_actionHighlight;
  //TODO other actions

  OptionsWidget *_menuOptions;
  TextEdit *_textEdit;

  bool _highlighted, _blockSignals;
  QButtonGroup *_alignmentButtons;
  QActionGroup *_alignmentActions;
};
