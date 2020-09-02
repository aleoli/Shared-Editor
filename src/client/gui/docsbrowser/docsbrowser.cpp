#include "docsbrowser.h"
#include "../ui/ui_docsbrowser.h"

#include <QAction>
#include <list>
#include <QGridLayout>
#include <cmath>

#include "dialogs/input.h"
#include "dialogs/confirm.h"
#include "dialogs/move.h"

#include "utils.h"
#include "docwidget.h"
#include "docwidget_folder.h"

#define DOC_WIDTH 200
#define DOC_HEIGHT 120

DocsBrowser::DocsBrowser(QWidget *parent): MainWindow(parent), ui(new Ui::DocsBrowser) {
  ui->setupUi(this);

  _widgetHome = ui->btn_home;
  _widgetNewFolder = ui->btn_new_folder;

  _widgetBack = ui->btn_back;
  _widgetForward = ui->btn_fwd;
  _widgetUp = ui->btn_up;

  _widgetAccount = ui->btn_logout;
  _widgetNewFile = ui->btn_new_file;
  _widgetOpenLink = ui->btn_open_link;
  _widgetSearch = ui->ledit_search;

  _sortComboBox = ui->comboBox_order;

  _actionLogout = ui->actionLogout;

  _scrollArea = ui->scrollArea_docs;
  _layout = new QGridLayout{_scrollArea};
  _layout->setHorizontalSpacing(this->_getHSpacing());
  _layout->setVerticalSpacing(50);
  _layout->setSizeConstraint(QLayout::SetFixedSize);

  _widgetPath = ui->pathWidget;

  connect(_widgetHome, &QPushButton::clicked, this, &DocsBrowser::_goToHome);
  connect(_widgetNewFolder, &QPushButton::clicked, this, &DocsBrowser::_newDir);
  connect(_widgetOpenLink, &QPushButton::clicked, this, &DocsBrowser::_openLink);

  connect(_widgetBack, &QPushButton::clicked, this, &DocsBrowser::_goBack);
  connect(_widgetForward, &QPushButton::clicked, this, &DocsBrowser::_goFwd);
  connect(_widgetUp, &QPushButton::clicked, this, &DocsBrowser::_goUp);

  connect(_widgetAccount, &QPushButton::clicked, this, &DocsBrowser::_account);
  connect(_widgetNewFile, &QPushButton::clicked, this, &DocsBrowser::_newFile);

  connect(_actionLogout, &QAction::triggered, this, &DocsBrowser::_logout);

  connect(_sortComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &DocsBrowser::_changeSortType);
}

DocsBrowser::~DocsBrowser() {
  delete ui;
}

void DocsBrowser::clear() {
  this->_widgetSearch->setText("");
  setIcon();

  emit this->_goToHome(true);
}

void DocsBrowser::setIcon() {
  _widgetAccount->setIcon(_user->getIcon());
  _widgetAccount->setIconSize(_widgetAccount->size());
}

void DocsBrowser::refresh() {
  if(this->_currentDir) {
    this->changeDir(*this->_currentDir);
  }
}

void DocsBrowser::getAllDirsResponse(std::list<std::pair<QString, int>> items) {
  items.remove_if([this](auto& el) {
    return this->_menuElement.getId() == el.second || this->_menuElement.getParentId() == el.second || el.first.startsWith(this->_menuElement.getName() + "/") || el.first.contains("/"+this->_menuElement.getName()+"/");
  });
  auto newFolder = Move::show(items, this, "Move file", "Cancel", "Confirm");
  if(newFolder && *newFolder > 0) {
    debug("Move to: " + QString::number(*newFolder));
    emit this->move(_user->getToken(), this->_menuElement.getId(), *newFolder);
  }
}

void DocsBrowser::showDir(const std::vector<FSElement> &elements, const QString& name, int parentId) {
  if(parentId == -1) {
    this->_currentParent = std::nullopt;
  } else {
    this->_currentParent = parentId;
  }

  this->_showDir(elements);
}

void DocsBrowser::_showDir(const std::vector<FSElement> &elements) {
  this->_cleanWidgets();
  int nCols = this->_get_n_cols();

  auto sortType = this->_currentSortType;
  std::list<const FSElement*> listPointers{};
  for(const auto& element: elements) {
    listPointers.push_back(&element);
  }
  listPointers.sort([sortType](auto first, auto second) {
      switch(sortType) {
        case TITLE:
          return first->getName() < second->getName();
        case DATE:
          return first->getCreationDate() > second->getCreationDate();
      }
      // it will be never reached
      return first->getName() < second->getName();
  });

  int i = 0;
  for(auto element = listPointers.begin(); element != listPointers.end(); ++element, i++) {
    //debug(QString(element.getType() == FSElement::Type::FILE ? "FILE" : "DIR") + "\t" + element.getName());

    if((*element)->getType() == FSElement::Type::FILE) {
      auto widget = new DocWidget{**element, this->_scrollArea};
      widget->setFixedSize(DOC_WIDTH, DOC_HEIGHT);
      QObject::connect(widget, &DocWidget::open, this, &DocsBrowser::_openFile);
      QObject::connect(widget, &DocWidget::openMenu, this, &DocsBrowser::_openMenu);

      this->_currentWidgets.push_back(widget);

      int col = i % nCols;
      int row = floor(i / nCols);
      this->_layout->addWidget(widget, row, col, QFlags<Qt::AlignmentFlag>{Qt::AlignCenter, Qt::AlignVCenter});
    } else {
      auto widget = new DocWidgetFolder{**element, this->_scrollArea};
      widget->setFixedSize(DOC_WIDTH, DOC_HEIGHT);
      QObject::connect(widget, &DocWidgetFolder::open, this, &DocsBrowser::changeDir);
      QObject::connect(widget, &DocWidgetFolder::openMenu, this, &DocsBrowser::_openMenu);

      this->_currentWidgetsFolder.push_back(widget);

      int col = i % nCols;
      int row = floor(i / nCols);
      this->_layout->addWidget(widget, row, col, QFlags<Qt::AlignmentFlag>{Qt::AlignCenter, Qt::AlignVCenter});
    }
  }
  this->_currentElements = elements;

  // print current history
  debug("History");
  std::for_each(this->_dirHistory.begin(), this->_dirHistory.end(), [](const auto& item) {
      debug(QString::number(item));
  });
  debug("");
}

void DocsBrowser::_openFile(int fileId) {
  _user->setFileId(fileId);
  emit this->openFile(_user->getToken(), fileId);
}

void DocsBrowser::changeDir(int dirId) {
  // TODO: freeze window
  debug("go to dir " + QString::number(dirId));
  // get current index in history
  if(this->_currentDir) {
    this->_dirHistory.erase(std::next(this->_curInHistory), this->_dirHistory.end());
  }

  this->_currentDir = dirId;
  if(this->_dirHistory.empty() || this->_dirHistory.back() != *this->_currentDir) {
    this->_dirHistory.push_back(*this->_currentDir);
  }
  this->_curInHistory = --this->_dirHistory.end();

  emit this->getDir(_user->getToken(), dirId);
  emit this->getPath(_user->getToken(), *this->_currentDir);
}

void DocsBrowser::showPath(const std::vector<FSElement> &elements) {
  qDeleteAll(this->_widgetPath->children());

  auto layout = new QHBoxLayout{this->_widgetPath};
  layout->setSizeConstraint(QLayout::SetFixedSize);
  for(auto el = elements.crbegin(); el != elements.crend(); ++el) {
    debug(el->getName());

    if(el->getId() != 1) {
      auto btn = new QPushButton{el->getName(), this->_widgetPath};
      btn->setStyleSheet("outline: none; background: transparent; font: 18px; color: white; padding: 1 3 1 3; border-width: 0px; border-radius: 3px; text-align: left;");
      btn->setObjectName(QString::number(el->getId()));
      btn->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
      btn->setMaximumSize(btn->sizeHint().width(), btn->sizeHint().height());

      QObject::connect(btn, &QPushButton::clicked, this, &DocsBrowser::_clickedPath);

      layout->addWidget(btn);
    }

    auto lbl = new QLabel{"/", this->_widgetPath};
    lbl->setStyleSheet("border: none; color: white; font: 24px;");
    lbl->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    lbl->setMaximumSize(lbl->sizeHint().width(), lbl->sizeHint().height());
    layout->addWidget(lbl);
  }

  _widgetPath->setLayout(layout);
}

void DocsBrowser::_clickedPath(bool checked) {
  auto btn = qobject_cast<QPushButton*>(sender());
  if(btn) {
    bool ok;
    auto elId = btn->objectName().toInt(&ok);
    if(!ok) {
      return;
    }
    this->_currentDir = elId;
    emit this->getDir(_user->getToken(), elId);
    emit this->getPath(_user->getToken(), *this->_currentDir);
  }
}

void DocsBrowser::_changeSortType(int newSortType) {
  if(newSortType < SortType::TITLE || newSortType > SortType::DATE) {
    warn("Invalid sort type");
    return;
  }
  this->_currentSortType = static_cast<SortType>(newSortType);
  this->_showDir(this->_currentElements);
}

void DocsBrowser::_openLink(bool checked) {
  auto link = Input::show(this, "Insert your link", "", "Cancel", "Open");
  if(link && !link->isEmpty()) {
    emit activateLink(_user->getToken(), *link);
  }
}

void DocsBrowser::_account(bool checked) {
  auto actionEdit = new QAction{QIcon(":res/rename.png"), "Edit Account"};
  auto actionLogout = new QAction{QIcon(":res/logout.png"), "Logout"};

  QMenu menu;
  menu.addAction(actionEdit);
  menu.addAction(actionLogout);
  auto action = menu.exec(QCursor::pos());

  if(!action) return;

  if(action == actionEdit) {
    emit editAccount();
  } else if(action == actionLogout) {
    emit logout(_user->getToken());
  }
}

void DocsBrowser::_newFile(bool checked) {
  // TODO: freeze window
  auto name = Input::show(this, "Insert file name", "", "Cancel", "Create");
  if(name && !name->isEmpty()) {
    emit newFile(_user->getToken(), *name, _currentDir);
  }
}

void DocsBrowser::_newDir(bool checked) {
  // TODO: freeze window
  auto name = Input::show(this, "Insert directory name", "", "Cancel", "Create");
  if(name && !name->isEmpty()) {
    emit newDir(_user->getToken(), *name, _currentDir);
  }
}

void DocsBrowser::_logout(bool checked) {
  emit logout(_user->getToken());
}

void DocsBrowser::_goToHome(bool checked) {
  debug("go to home");
  this->changeDir(1);
}

void DocsBrowser::_goBack(bool checked) {
  if(this->_curInHistory == this->_dirHistory.begin()) {
    // is first in list, do nothing
  } else {
    this->_curInHistory--;
    this->_currentDir = *this->_curInHistory;
    emit this->getDir(_user->getToken(), *this->_currentDir);
    emit this->getPath(_user->getToken(), *this->_currentDir);
  }
}

void DocsBrowser::_goFwd(bool checked) {
  if(std::next(this->_curInHistory) == this->_dirHistory.end()) {
    // is last in list, do nothing
  } else {
    this->_curInHistory++;
    this->_currentDir = *this->_curInHistory;
    emit this->getDir(_user->getToken(), *this->_currentDir);
    emit this->getPath(_user->getToken(), *this->_currentDir);
  }
}

void DocsBrowser::_goUp(bool checked) {
  if(this->_currentParent) {
    this->changeDir(*this->_currentParent);
  }
}

std::list<int>::const_iterator DocsBrowser::_getCurrent() {
  return this->_curInHistory;
}

void DocsBrowser::_cleanWidgets() {
  for(auto& widget: this->_currentWidgets) {
    QObject::disconnect(widget, &DocWidget::open, this, &DocsBrowser::_openFile);
    QObject::disconnect(widget, &DocWidget::openMenu, this, &DocsBrowser::_openMenu);
    widget->hide();
    widget->setParent(nullptr);
    delete widget;
  }
  for(auto& widget: this->_currentWidgetsFolder) {
    QObject::disconnect(widget, &DocWidgetFolder::open, this, &DocsBrowser::changeDir);
    QObject::disconnect(widget, &DocWidgetFolder::openMenu, this, &DocsBrowser::_openMenu);
    widget->hide();
    widget->setParent(nullptr);
    delete widget;
  }
  this->_currentWidgets.clear();
  this->_currentWidgetsFolder.clear();
}

void DocsBrowser::resizeEvent(QResizeEvent *event) {
  debug("resize");
  this->_showDir(this->_currentElements);
}

int DocsBrowser::_get_n_cols() {
  auto width = this->width();
  auto hSpacing = this->_getHSpacing();
  _layout->setHorizontalSpacing(hSpacing);
  return floor(width / (DOC_WIDTH + hSpacing));
}

int DocsBrowser::_getHSpacing() {
  auto width = this->width();
  auto minSpacing = 30;
  auto docHArea = DOC_WIDTH + minSpacing;
  int n = width / docHArea;
  return (width - (n * DOC_WIDTH)) / (n+1);
}

void DocsBrowser::_openMenu(bool isDir, const FSElement& element) {
  debug(QString("Open menu ") + (isDir ? "DIR" : "FILE"));

  auto actionMove = new QAction{QIcon(":res/move.png"), "Move"};
  QAction *actionShare = nullptr;
  if(!isDir) {
    actionShare = new QAction{QIcon(":res/share.png"), "Share"};
  }
  auto actionRename = new QAction{QIcon(":res/rename.png"), "Rename"};
  auto actionDelete = new QAction{QIcon(":res/delete.png"), "Delete"};

  QMenu menu;
  menu.addAction(actionMove);
  if(!isDir) {
    menu.addAction(actionShare);
  }
  menu.addAction(actionRename);
  menu.addAction(actionDelete);
  auto action = menu.exec(QCursor::pos());

  if(!action) return;

  if(action == actionMove) {
    debug("Move");
    this->_menuElement = FSElement{element};
    emit this->getAllDirs(_user->getToken());
  } else if(action == actionShare) {
    debug("Share");
    emit this->share(_user->getToken(), _user->getFileId());
  } else if(action == actionRename) {
    debug("Rename");
    auto name = Input::show(this, "Insert new name", element.getName(), "Cancel", "Rename");
    if(name && !name->isEmpty() && element.getName() != *name) {
      emit this->edit(_user->getToken(), element.getId(), name);
    }
  } else if(action == actionDelete) {
    debug("Delete");
    if(Confirm::show(this, "Are you sure to delete this file?")) {
      if(isDir) {
        emit this->deleteDir(_user->getToken(), element.getId());
      } else {
        emit this->deleteFile(_user->getToken(), element.getId());
      }
    }
  }
}
