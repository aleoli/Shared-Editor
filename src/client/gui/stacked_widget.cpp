#include "stacked_widget.h"

#include <algorithm>

#include "utils.h"
#include "exceptions.h"

using namespace se_exceptions;

StackedWidget::StackedWidget(QObject *parent) : QObject(parent), _current(nullptr), _show(false) {}

StackedWidget::~StackedWidget() {
  for(MainWindow *widget : _widgets) {
    delete widget;
  }
}

void StackedWidget::show() {
  if(_current == nullptr) {
    throw GuiException{"show failed: no widgets in the StackedWidget!"};
  }

  if(_show) return;

  _current->show();
  _show = true;
}

void StackedWidget::hide() {
  if(_current == nullptr) {
    throw GuiException{"hide failed: no widgets in the StackedWidget!"};
  }

  _current->hide();
  _show = false;
}

bool StackedWidget::close() {
  if(_current == nullptr) {
    throw GuiException{"close failed: no widgets in the StackedWidget!"};
  }

  _show = false;
  return _current->close();
}

void StackedWidget::addWidget(MainWindow *widget) {
  if(_widgets.empty()) {
    setCurrent(widget);
  }
  else {
    auto it = std::find(_widgets.begin(), _widgets.end(), widget);
    if (it != _widgets.end()) {
      throw GuiException{"addWidget failed: element already present!"};
    }
  }

  _widgets.push_back(widget);
  connect(widget, &MainWindow::exit, this, &StackedWidget::quit);
}

void StackedWidget::setCurrentWidget(MainWindow *widget) {
  if(_current == widget) return;

  auto it = std::find(_widgets.begin(), _widgets.end(), widget);
  if (it == _widgets.end()) {
    throw GuiException{"setCurrentWidget failed: element not present!"};
  }

  setCurrent(widget);
}

MainWindow *StackedWidget::currentWidget() const {
  return _current;
}

void StackedWidget::setCurrent(MainWindow *widget) {
  if(_show) {
    _current->hide();
    widget->move(_current->pos());
    widget->show();
  }

    _current = widget;
}
