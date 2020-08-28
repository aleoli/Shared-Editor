#pragma once

#include "alert_messages.h"
#include "user.h"
#include "main_window.h"
#include <FSElement.h>
#include "docwidget.h"
#include "docwidget_folder.h"

#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
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
  void showDir(const std::vector<FSElement> &elements, const QString& name, int parentId);
  void changeDir(int dirId);
  void showPath(const std::vector<FSElement> &elements);

signals:
  void logout(const QString &token);
  void editAccount();
  void newFile(const QString &token, const QString &name, const std::optional<int> &dirId = std::nullopt);
  void newDir(const QString &token, const QString &name, const std::optional<int> &dirId = std::nullopt);
  void activateLink(const QString &token, const QString &link);

  void getDir(const QString &token, std::optional<int> dirId = std::nullopt);
  void openFile(const QString &token, int fileId);
  void getPath(const QString &token, int dirId);

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
  void _clickedPath(bool checked);
  void _changeSortType(int newSortType);

  void _openFile(int fileId);

private:
  typedef enum {
    TITLE, DATE
  } SortType;

  Ui::DocsBrowser *ui;

  QPushButton *_widgetHome;
  QPushButton *_widgetNewFolder;
  QPushButton *_widgetBack, *_widgetForward, *_widgetUp;

  QPushButton *_widgetAccount, *_widgetNewFile, *_widgetSearchButton;
  QLineEdit *_widgetSearch;
  QAction *_actionLogout;

  QComboBox *_sortComboBox;

  QWidget *_scrollArea;
  QGridLayout *_layout;
  QWidget *_widgetPath;

  std::optional<int> _currentParent = std::nullopt;
  std::optional<int> _currentDir = std::nullopt;
  std::list<int> _dirHistory{};
  std::list<int>::iterator _curInHistory;

  std::vector<FSElement> _currentElements{};

  std::list<DocWidget *> _currentWidgets{};
  std::list<DocWidgetFolder *> _currentWidgetsFolder{};

  SortType _currentSortType = TITLE;

  std::list<int>::const_iterator _getCurrent();
  void _cleanWidgets();
  int _get_n_cols();
  void _showDir(const std::vector<FSElement> &elements);
  int _getHSpacing();
};
