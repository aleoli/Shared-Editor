#pragma once

#include "alert_messages.h"
#include "user.h"
#include "main_window.h"
#include <FSElement.h>
#include "docwidget.h"
#include "docwidget_folder.h"

#include <QPushButton>
#include <QLineEdit>
#include <QGridLayout>

#include <optional>

namespace Ui {
    class DocsBrowser;
}

class DocsBrowser : public MainWindow {
  Q_OBJECT

public:
  explicit DocsBrowser(QWidget *parent = nullptr);
  ~DocsBrowser();

public slots:
  virtual void clear();

  void setIcon();
  void showDir(const std::vector<FSElement> &elements);
  void changeDir(int dirId);

signals:
  void logout(const QString &token);
  void editAccount();
  void newFile(const QString &token, const QString &name, const std::optional<int> &dirId = std::nullopt);
  void newDir(const QString &token, const QString &name, const std::optional<int> &dirId = std::nullopt);
  void activateLink(const QString &token, const QString &link);

  void getDir(const QString &token, std::optional<int> dirId = std::nullopt);

protected:
  void resizeEvent(QResizeEvent *event) override;

private slots:
  void _account(bool checked);
  void _newFile(bool checked);
  void _newDir(bool checked);
  void _logout(bool checked);
  void _search(bool checked);
  void _goToHome(bool checked);
  void _goBack(bool checked);
  void _goFwd(bool checked);
  void _goUp(bool checked);

  void _openFile(int fileId);

private:
  Ui::DocsBrowser *ui;

  QPushButton *_widgetHome;
  QPushButton *_widgetNewFolder;
  QPushButton *_widgetBack, *_widgetForward, *_widgetUp;

  QPushButton *_widgetAccount, *_widgetNewFile, *_widgetSearchButton;
  QLineEdit *_widgetSearch;
  QAction *_actionLogout;

  QWidget *_scrollArea;
  QGridLayout *_layout;

  std::optional<int> _currentDir = std::nullopt;
  std::list<int> _dirHistory{};

  std::vector<FSElement> _currentElements{};

  std::list<DocWidget *> _currentWidgets{};
  std::list<DocWidgetFolder *> _currentWidgetsFolder{};

  std::list<int>::const_iterator _getCurrent();
  void _cleanWidgets();
  int _get_n_cols();
};
