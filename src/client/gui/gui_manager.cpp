#include "gui_manager.h"

#include <iostream>

#include "exceptions.h"
#include "utils.h"

#include "login.h"
#include "docsbrowser.h"
#include "edit.h"
#include "landing.h"
#include "registration.h"
#include "texteditor.h"

using namespace se_exceptions;

std::shared_ptr<GuiManager> GuiManager::instance = nullptr;

GuiManager::GuiManager(const SysConf &conf, QObject *parent): QObject(parent) {
  initThreads(conf);

  _stackedWidget = new MyStackedWidget();

  //TODO
}

GuiManager::~GuiManager() {
  delete _stackedWidget;
}

std::shared_ptr<GuiManager> GuiManager::get(std::optional<SysConf> conf) {
  if(instance == nullptr) {
    if(conf)
      instance.reset(new GuiManager(*conf));
    else
      throw GuiException{"No configuration provided!"};
  }
  return instance;
}

void GuiManager::initThreads(const SysConf &conf) {
  //TODO
}

void GuiManager::run() {
  //TODO
  //Test
  auto widget = new Login();
  widget->show();
}
