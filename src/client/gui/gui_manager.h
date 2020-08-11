#ifndef GUIMANAGER_H
#define GUIMANAGER_H

#include <QStackedWidget>
#include <QObject>
#include <memory>
#include <optional>
#include <QCloseEvent>

#include "../message_manager.h"
#include "../server.h"
#include "../sys.h"
#include "user.h"

class MyStackedWidget: public QStackedWidget
{
  Q_OBJECT

public:
  void closeEvent(QCloseEvent *bar) {
    emit wannaQuit();
    bar->ignore();
  }

signals:
  void wannaQuit(); //TODO vedi se ti serve sto robo
};

class GuiManager: public QObject
{
  Q_OBJECT

public:
  GuiManager(const GuiManager&) = delete;
  GuiManager(GuiManager&&) = delete;
  GuiManager& operator=(const GuiManager&) = delete;
  GuiManager& operator=(GuiManager&&) = delete;

  ~GuiManager();
  static std::shared_ptr<GuiManager> get(std::optional<SysConf> conf = std::nullopt);

  void run();

signals:
  void quit();

private slots:
  // connessione del server
  void connected();
  void connectionLost();

private:
  static std::shared_ptr<GuiManager> instance;
  explicit GuiManager(const SysConf &conf, QObject *parent = nullptr);

  void initThreads(const SysConf &conf);

  MyStackedWidget *_stackedWidget;
  std::shared_ptr<MessageManager> _manager;
  std::shared_ptr<Server> _server;
  QThread *_serverThread, *_managerThread;

  std::shared_ptr<User> _user;
};
#endif // GUIMANAGER_H
