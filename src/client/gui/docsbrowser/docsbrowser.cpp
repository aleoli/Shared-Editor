#include "docsbrowser.h"
#include "../ui/ui_docsbrowser.h"

#include <QAction>
#include <QInputDialog>
#include <list>
#include <QGridLayout>
#include <cmath>

#include "utils.h"
#include "docwidget.h"
#include "docwidget_folder.h"

DocsBrowser::DocsBrowser(QWidget *parent): MainWindow(parent), ui(new Ui::DocsBrowser) {
  ui->setupUi(this);

  _widgetHome = findChild<QPushButton *>("btn_home");
  _widgetNewFolder = findChild<QPushButton *>("btn_new_folder");

  _widgetBack = findChild<QPushButton *>("btn_back");
  _widgetForward = findChild<QPushButton *>("btn_fwd");
  _widgetUp = findChild<QPushButton *>("btn_up");

  _widgetAccount = findChild<QPushButton *>("btn_logout");
  _widgetNewFile = findChild<QPushButton *>("btn_new_file");
  _widgetSearchButton = findChild<QPushButton *>("btn_search");
  _widgetSearch = findChild<QLineEdit *>("ledit_search");

  _actionLogout = findChild<QAction *>("actionLogout");

  _scrollArea = findChild<QWidget *>("scrollArea_docs");
  _layout = new QGridLayout{_scrollArea};
  _layout->setSpacing(50);
  _layout->setVerticalSpacing(50);

  connect(_widgetHome, &QPushButton::clicked, this, &DocsBrowser::_goToHome);
  connect(_widgetNewFolder, &QPushButton::clicked, this, &DocsBrowser::_newDir);

  connect(_widgetBack, &QPushButton::clicked, this, &DocsBrowser::_goBack);
  connect(_widgetForward, &QPushButton::clicked, this, &DocsBrowser::_goFwd);
  connect(_widgetUp, &QPushButton::clicked, this, &DocsBrowser::_goUp);

  connect(_widgetAccount, &QPushButton::clicked, this, &DocsBrowser::_account);
  connect(_widgetNewFile, &QPushButton::clicked, this, &DocsBrowser::_newFile);
  connect(_widgetSearchButton, &QPushButton::clicked, this, &DocsBrowser::_search);

  connect(_actionLogout, &QAction::triggered, this, &DocsBrowser::_logout);
}

DocsBrowser::~DocsBrowser() {
  delete ui;
}

void DocsBrowser::clear() {
  // TODO Cose da fare quando la finestra è portata in primo piano (es. svuotare i campi)
  setIcon();

  emit this->_goToHome(true);
}

void DocsBrowser::setIcon() {
  _widgetAccount->setIcon(_user->getIcon());
  _widgetAccount->setIconSize(_widgetAccount->size());
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
  for(int i=0; i<elements.size(); i++) {
    auto element = elements[i];
    debug(QString(element.getType() == FSElement::Type::FILE ? "FILE" : "DIR") + "\t" + element.getName());

    if(element.getType() == FSElement::Type::FILE) {
      auto widget = new DocWidget{std::move(element), this->_scrollArea};
      widget->setFixedSize(200, 120);
      QObject::connect(widget, &DocWidget::open, this, &DocsBrowser::_openFile);

      this->_currentWidgets.push_back(widget);

      int col = i % nCols;
      int row = floor(i / nCols);
      this->_layout->addWidget(widget, row, col, QFlags<Qt::AlignmentFlag>{Qt::AlignCenter, Qt::AlignVCenter});
    } else {
      auto widget = new DocWidgetFolder{std::move(element), this->_scrollArea};
      widget->setFixedSize(200, 120);
      QObject::connect(widget, &DocWidgetFolder::open, this, &DocsBrowser::changeDir);

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
  debug("TODO");
  debug("open file " + QString::number(fileId));
}

void DocsBrowser::changeDir(int dirId) {
  // TODO: freeze window
  debug("go to dir " + QString::number(dirId));
  // get current index in history
  if(this->_currentDir) {
    auto it = std::find(this->_dirHistory.crbegin(), this->_dirHistory.crend(), this->_currentDir);
    this->_dirHistory.erase(it.base(), this->_dirHistory.end());
  }

  this->_currentDir = dirId;
  this->_dirHistory.push_back(*this->_currentDir);

  emit this->getDir(_user->getToken(), dirId);
}

void DocsBrowser::_account(bool checked) {
  //TODO qua dovrebbe comparire un menu a tendina con le opzioni edit account e logout
  emit editAccount();
  //emit logout(_user->getToken());
}

void DocsBrowser::_newFile(bool checked) {
  // TODO: freeze window
  bool ok;
  QString name = QInputDialog::getText(this, "New File", "Insert file name", QLineEdit::Normal, "", &ok);
  if(ok && !name.isEmpty()) {
    emit newFile(_user->getToken(), name, _currentDir);
  }
}

void DocsBrowser::_newDir(bool checked) {
  // TODO: freeze window
  bool ok;
  QString name = QInputDialog::getText(this, "New Directory", "Insert directory name", QLineEdit::Normal, "", &ok);
  if(ok && !name.isEmpty()) {
    emit newDir(_user->getToken(), name, _currentDir);
  }
}

void DocsBrowser::_logout(bool checked) {
  emit logout(_user->getToken());
}

void DocsBrowser::_search(bool checked) {
  //TODO qua metto la logica per attivare un link, per i miei test, ovviamente cambiala
  auto username = _widgetSearch->text();
  emit activateLink(_user->getToken(), username);
}

void DocsBrowser::_goToHome(bool checked) {
  debug("go to home");
  this->changeDir(1);
}

void DocsBrowser::_goBack(bool checked) {
  auto curr = this->_getCurrent();
  if(curr-- == this->_dirHistory.begin()) {
    // is first in list, do nothing
  } else {
    this->_currentDir = *curr;
    emit this->getDir(_user->getToken(), *curr);
  }
}

void DocsBrowser::_goFwd(bool checked) {
  auto curr = this->_getCurrent();
  if(++curr == this->_dirHistory.end()) {
    // is last in list, do nothing
  } else {
    this->_currentDir = *curr;
    emit this->getDir(_user->getToken(), *curr);
  }
}

void DocsBrowser::_goUp(bool checked) {
  if(this->_currentParent) {
    this->changeDir(*this->_currentParent);
  }
}

std::list<int>::const_iterator DocsBrowser::_getCurrent() {
  return std::next(std::find(this->_dirHistory.crbegin(), this->_dirHistory.crend(), this->_currentDir)).base();
}

void DocsBrowser::_cleanWidgets() {
  for(auto& widget: this->_currentWidgets) {
    QObject::disconnect(widget, &DocWidget::open, this, &DocsBrowser::_openFile);
    widget->hide();
    widget->setParent(nullptr);
    delete widget;
  }
  for(auto& widget: this->_currentWidgetsFolder) {
    QObject::disconnect(widget, &DocWidgetFolder::open, this, &DocsBrowser::changeDir);
    widget->hide();
    widget->setParent(nullptr);
    delete widget;
  }
  this->_currentWidgets.clear();
  this->_currentWidgetsFolder.clear();
}

int DocsBrowser::_get_n_cols() {
  auto width = this->width();
  return floor(width / (200 + 50));
}

void DocsBrowser::resizeEvent(QResizeEvent *event) {
  debug("resize");
  this->_showDir(this->_currentElements);
}
