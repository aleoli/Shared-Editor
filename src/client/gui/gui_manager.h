#pragma once

#include <QObject>
#include <memory>
#include <optional>

#include "message_manager.h"
#include "stacked_widget.h"
#include "server.h"
#include "sys.h"
#include "utils.h"
#include "user.h"
#include "login.h"
#include "docsbrowser.h"
#include "edit.h"
#include "registration.h"
#include "texteditor.h"
#include "alert_messages.h"
#include "FSElement.h"

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

  void loginQuery(const QString &username, const QString &password);
  void logoutQuery(const QString &token);
  void newUserQuery(const QString &username, const QString &password, const QString &pswRepeat, const std::optional<QString> &icon);
  void editUserQuery(const QString &token, const std::optional<QString> &oldPassword, const std::optional<QString> &password, const std::optional<QString> &pswRepeat, const std::optional<QString> &icon);
  void deleteUserQuery(const QString &token);

  void newFileQuery(const QString &token, const QString &name, const std::optional<int> &dirId = std::nullopt);
  void getFileQuery(const QString &token, int fileId);
  void closeFileQuery(const QString &token, int fileId);
  void editFileQuery(const QString &token, int fileId, const std::optional<QString> &name);
  void deleteFileQuery(const QString &token, int fileId);
  void getLinkQuery(const QString &token, int fileId);
  void activateLinkQuery(const QString &token, const QString &link);

  void localInsertQuery(const QString &token, int fileId, const std::vector<Symbol> &symbols);
  void localDeleteQuery(const QString &token, int fileId, const std::vector<SymbolId> &ids);
  void localUpdateQuery(const QString &token, int fileId, const std::vector<Symbol> &symbols);
  void localMoveQuery(const QString &token, int fileId, const SymbolId &symbolId, int cursorPosition);

  void newDirQuery(const QString &token, const QString &name, const std::optional<int> &parentId = std::nullopt);
  void editDirQuery(const QString &token, int dirId, const std::optional<QString> &name, const std::optional<int> &parentId);
  void deleteDirQuery(const QString &token, int dirId);
  void getDirQuery(const QString &token, const std::optional<int> &dirId = std::nullopt);
  void moveFileQuery(const QString &token, int fileId, int dirId);

  void commentLocalInsertQuery(const QString &token, int fileId, const File::Comment &comment);
  void commentLocalUpdateQuery(const QString &token, int fileId, const File::Comment &comment);
  void commentLocalDeleteQuery(const QString &token, int fileId, const File::Comment &comment);

public slots:
  void closeStacked();
  void alert(Alert type, const QString &what, const QString &title = "");

private slots:
  // connessione del server
  void connected();
  void connectionLost();

  // messages from windows
  void loginLogin(const QString &username, const QString &password);
  void loginSignup();

  void registrationSignup(const QString &username, const QString &password, const QString &pswRepeat, const std::optional<QString> &icon);
  void registrationCancel();

  void editSave(const QString &token, const std::optional<QString> &oldPassword,
    const std::optional<QString> &password, const std::optional<QString> &pswRepeat, const std::optional<QString> &icon);
  void editCancel();
  void editDelete(const QString &token);

  void docsBrowserNewFile(const QString &token, const QString &name, const std::optional<int> &dirId = std::nullopt);
  void docsBrowserLogout(const QString &token);
  void docsBrowserEditAccount();
  void docsBrowserActivateLink(const QString &token, const QString &link);

  void textEditorLogout(const QString &token);
  void textEditorShare(const QString &token, int fileId);
  void textEditorClose(const QString &token, int fileId);
  void textEditorEdit(const QString &token, int fileId, const std::optional<QString> &name);
  void textEditorRemove(const QString &token, int fileId);

  // messages from server
  void serverErrorResponse(const QString &reason);

  void serverLoginResponse(const QString &token, int userId, const std::optional<QString> &icon);
  void serverNewUserResponse(const QString &token, int userId);
  void serverEditUserResponse();
  void serverDeleteUserResponse();

  void serverNewFileResponse(int fileId);
  void serverActivateLinkResponse(const FSElement &element, const File &file);
  void serverDeleteFileResponse();

  void serverGetLinkResponse(const QString &link);

private:
  void showWindow(MainWindow *window, bool clear = false);
  void freezeWindow();
  void unfreezeWindow();

  static std::shared_ptr<GuiManager> instance;
  explicit GuiManager(const SysConf &conf, QObject *parent = nullptr);

  void initThreads(const SysConf &conf);
  void connectWidgets();
  void connectClientToServer();
  void connectServerToClient();

  std::shared_ptr<MessageManager> _manager;
  std::shared_ptr<Server> _server;
  QThread *_serverThread, *_managerThread;

  std::shared_ptr<User> _user;
  StackedWidget *_stackedWidget;

  //finestre
  Login *_widgetLogin;
  DocsBrowser *_widgetDocsBrowser;
  Edit *_widgetEdit;
  Registration *_widgetRegistration;
  TextEditor *_widgetTextEditor;
};
