#pragma once

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QIcon>

namespace Ui {
class Options;
}

class OptionsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit OptionsWidget(QWidget *parent = nullptr);
    ~OptionsWidget();

signals:
  void share();
  void comments(bool checked);
  void connected(bool checked);
  void user();
  void rename(const QString &name);

public slots:
  void setFileName(const QString &name);
  void setShowCommentsChecked(bool checked);
  void setShowConnectedChecked(bool checked);
  void setIcon(const QIcon &icon);
  void focusRename();

private slots:
  void _share(bool checked);
  void _comments(bool checked);
  void _connected(bool checked);
  void _user(bool checked);
  void _rename();

private:
    Ui::Options *ui;

    QPushButton *_widgetShare, *_widgetComments, *_widgetConnected, *_widgetUser;
    QLineEdit *_widgetRename;
};
