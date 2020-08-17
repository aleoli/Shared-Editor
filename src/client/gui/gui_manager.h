#pragma once

#include <QStackedWidget>
#include <QObject>
#include <memory>
#include <optional>
#include <QCloseEvent>

#include "message_manager.h"
#include "server.h"
#include "sys.h"
#include "utils.h"
#include "user.h"
#include "login.h"
#include "docsbrowser.h"
#include "edit.h"
#include "landing.h"
#include "registration.h"
#include "texteditor.h"
#include "alert_messages.h"

class MyStackedWidget: public QStackedWidget
{
  Q_OBJECT

public:
  void closeEvent(QCloseEvent *bar) {
    emit close();
    bar->ignore();
  }

signals:
  void close();
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

public slots:
  void closeStacked();
  void alert(Alert type, const QString &what);

private slots:
  // connessione del server
  void connected();
  void connectionLost();

  void loginSignup();
  void registrationCancel();
  void editCancel();
  void docsBrowserLogout();
  void docsBrowserEditAccount();

  // messages from server
  void serverErrorResponse(const QString &reason);
  void serverLoginResponse(const QString &token, int userId, const std::optional<QString> &icon);
  void serverNewUserResponse(const QString &token, int userId);
  void serverEditUserResponse();
  void serverNewFileResponse(int fileId);

private:
  // show widgets
  void showRegistration(bool clear = false);
  void showDocsBrowser(bool clear = false);
  void showLogin(bool clear = false);
  void showEdit(bool clear = false);
  void showTextEditor(bool clear = false);

  static std::shared_ptr<GuiManager> instance;
  explicit GuiManager(const SysConf &conf, QObject *parent = nullptr);

  void initThreads(const SysConf &conf);
  void connectWidgets();
  void connectServerToClient();

  std::shared_ptr<MessageManager> _manager;
  std::shared_ptr<Server> _server;
  QThread *_serverThread, *_managerThread;

  std::shared_ptr<User> _user;
  MyStackedWidget *_stackedWidget;

  //finestre
  Landing *_widgetLanding;
  Login *_widgetLogin;
  DocsBrowser *_widgetDocsBrowser;
  Edit *_widgetEdit;
  Registration *_widgetRegistration;
  TextEditor *_widgetTextEditor;
};
