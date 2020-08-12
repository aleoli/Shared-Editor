#include "disk_manager.h"

#include <QTimer>
#include <QThread>

std::shared_ptr<DiskManager> DiskManager::instance = nullptr;

DiskManager::DiskManager(QObject *parent): QObject(parent) {
  QTimer *timer = new QTimer(this);
  QObject::connect(timer, &QTimer::timeout, this, &DiskManager::save);
  QObject::connect(this, &DiskManager::quit, timer, &QTimer::stop);
  QObject::connect(this, &DiskManager::quit, this, &DiskManager::save);
  timer->start(30 * 1000);  // save every 30 seconds
}

DiskManager::~DiskManager() = default;

std::shared_ptr<DiskManager> DiskManager::get() {
  if(instance == nullptr) {
    instance.reset(new DiskManager{});
  }
  return instance;
}

void DiskManager::setFileMap(FifoMap<int, std::pair<QString, File>>* fileMap) {
  this->_fileMap = fileMap;
}

void DiskManager::save() {
  if(this->_fileMap == nullptr) {
    return;
  }
  info("flush files on disk");
  auto sl = std::shared_lock{this->_fileMap->getMutex()};
  for(auto& item: *this->_fileMap) {
    item.second.second.store(item.second.first);
  }
  info("flush completed");
}
