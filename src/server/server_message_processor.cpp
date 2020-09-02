#include "server_message_processor.h"
#include <QJsonDocument>
#include <utility>

#include "user.h"
#include "session.h"
#include "FSElement_db.h"
#include "utils.h"

#include "exceptions.h"
#include "errors.h"

using namespace se_exceptions;

ServerMessageProcessor::ServerMessageProcessor(const Message &m, quint64 clientId)
    : MessageProcessor(m), _clientId(clientId), _to_all(false) {
  _manager = MessageManager::get();
  try {
    this->process_message();
  } catch(MessageDataException &ex) {
    error(ex.what());
    this->sendErrorMsg(ex.what());
  } catch(SE_Exception &ex) {
    error(ex.what());
    this->sendErrorMsg(ex.what());
  } catch(...) {
    error(GENERIC_ERROR);
    this->sendErrorMsg(GENERIC_ERROR);
  }
}

bool ServerMessageProcessor::shouldSendToAll() const {
  return this->_to_all;
}

void ServerMessageProcessor::process_error() {
  // TODO: non so cosa farmene dell'errore lato server, disconnetto il client?!?
  info("ERROR message received");
  auto reason = _m.getString("reason");
  warn(reason);
}

void ServerMessageProcessor::process_user() {
  auto action = static_cast<Message::UserAction>(_m.getAction());
  bool isResponse = _m.getStatus() == Message::Status::RESPONSE;

  switch(action) {
    case Message::UserAction::LOGIN:
      if(isResponse)
        disconnect("Ricevuto messaggio con status non valido");
      else
        login();
      break;

    case Message::UserAction::LOGOUT:
      if(isResponse)
        disconnect("Ricevuto messaggio con status non valido");
      else
        logout();
      break;

    case Message::UserAction::NEW:
      if(isResponse)
        disconnect("Ricevuto messaggio con status non valido");
      else
        newUser();
      break;

    case Message::UserAction::EDIT:
      if(isResponse)
        disconnect("Ricevuto messaggio con status non valido");
      else
        editUser();
      break;

    case Message::UserAction::DELETE:
      if(isResponse)
        disconnect("Ricevuto messaggio con status non valido");
      else
        deleteUser();
      break;

    case Message::UserAction::GET_USER_ICON:
      if(isResponse)
        disconnect("Ricevuto messaggio con status non valido");
      else
        getUserIcon();
      break;

    default:
      disconnect("Ricevuto messaggio con azione non valida");
  }
}

void ServerMessageProcessor::process_file() {
  auto action = static_cast<Message::FileAction>(_m.getAction());
  bool isResponse = _m.getStatus() == Message::Status::RESPONSE;

  switch(action) {
    case Message::FileAction::NEW:
      if(isResponse)
        disconnect("Ricevuto messaggio con status non valido");
      else
        newFile();
      break;

    case Message::FileAction::GET:
      if(isResponse)
        disconnect("Ricevuto messaggio con status non valido");
      else
        getFile();
      break;

    case Message::FileAction::CLOSE:
      if(isResponse)
        disconnect("Ricevuto messaggio con status non valido");
      else
        closeFile();
      break;

    case Message::FileAction::EDIT:
      if(isResponse)
        disconnect("Ricevuto messaggio con status non valido");
      else
        editFile();
      break;

    case Message::FileAction::DELETE:
      if(isResponse)
        disconnect("Ricevuto messaggio con status non valido");
      else
        deleteFile();
      break;

    case Message::FileAction::GET_LINK:
      if(isResponse)
        disconnect("Ricevuto messaggio con status non valido");
      else
        getLink();
      break;

    case Message::FileAction::ACTIVATE_LINK:
      if(isResponse)
        disconnect("Ricevuto messaggio con status non valido");
      else
        activateLink();
      break;

    default:
      disconnect("Ricevuto messaggio con azione non valida");
  }
}

void ServerMessageProcessor::process_file_edit() {
  auto action = static_cast<Message::FileEditAction>(_m.getAction());
  bool isResponse = _m.getStatus() == Message::Status::RESPONSE;

  switch(action) {
    case Message::FileEditAction::LOCAL_INSERT:
      if(isResponse)
        disconnect("Ricevuto messaggio con status non valido");
      else
        localInsert();
      break;

    case Message::FileEditAction::LOCAL_DELETE:
      if(isResponse)
        disconnect("Ricevuto messaggio con status non valido");
      else
        localDelete();
      break;

    case Message::FileEditAction::LOCAL_UPDATE:
      if(isResponse)
        disconnect("Ricevuto messaggio con status non valido");
      else
        localUpdate();
      break;

    case Message::FileEditAction::LOCAL_MOVE:
      if(isResponse)
        disconnect("Ricevuto messaggio con status non valido");
      else
        localMove();
      break;

    default:
      disconnect("Ricevuto messaggio con azione non valida");
  }
}

void ServerMessageProcessor::process_comment() {
  auto action = static_cast<Message::CommentAction>(_m.getAction());
  bool isResponse = _m.getStatus() == Message::Status::RESPONSE;

  switch(action) {
    case Message::CommentAction::COMMENT_LOCAL_INSERT:
      if(isResponse)
        disconnect("Ricevuto messaggio con status non valido");
      else
        newComment();
      break;

    case Message::CommentAction::COMMENT_LOCAL_UPDATE:
      if(isResponse)
        disconnect("Ricevuto messaggio con status non valido");
      else
        updateComment();
      break;

    case Message::CommentAction::COMMENT_LOCAL_DELETE:
      if(isResponse)
        disconnect("Ricevuto messaggio con status non valido");
      else
        deleteComment();
      break;

    default:
      disconnect("Ricevuto messaggio con azione non valida");
  }
}

void ServerMessageProcessor::process_filesystem() {
  auto action = static_cast<Message::FileSystemAction>(_m.getAction());
  bool isResponse = _m.getStatus() == Message::Status::RESPONSE;

  switch(action) {
    case Message::FileSystemAction::NEW_DIR:
      if(isResponse)
        disconnect("Ricevuto messaggio con status non valido");
      else
        newDir();
      break;

    case Message::FileSystemAction::EDIT_DIR:
      if(isResponse)
        disconnect("Ricevuto messaggio con status non valido");
      else
        editDir();
      break;

    case Message::FileSystemAction::DELETE_DIR:
      if(isResponse)
        disconnect("Ricevuto messaggio con status non valido");
      else
        deleteDir();
      break;

    case Message::FileSystemAction::GET_DIR:
      if(isResponse)
        disconnect("Ricevuto messaggio con status non valido");
      else
        getDir();
      break;

    case Message::FileSystemAction::MOVE_FILE:
      if(isResponse)
        disconnect("Ricevuto messaggio con status non valido");
      else
        moveFile();
      break;

    case Message::FileSystemAction::GET_PATH:
      if(isResponse)
        disconnect("Ricevuto messaggio con status non valido");
      else
        getPath();
      break;

    case Message::FileSystemAction::GET_ALL_DIRS:
      if(isResponse)
        disconnect("Ricevuto messaggio con status non valido");
      else
        getAllDirs();
      break;

    default:
      disconnect("Ricevuto messaggio con azione non valida");
  }
}

void ServerMessageProcessor::disconnect(const QString& why) {
  error(why);
  // TODO
  // emit _manager->connection_error();
}

void ServerMessageProcessor::sendErrorMsg(const QString& reason) {
  QJsonObject data;
  data["reason"] = reason;

  this->_res = Message{Message::Type::ERROR, 0, Message::Status::RESPONSE, data};
  this->_has_resp = true;
}





// ------ handlers ------

void ServerMessageProcessor::login() {
  info("Login query received");

  std::shared_ptr<Session> s;

  try {
    auto username = _m.getString("username");
    auto password = _m.getString("password");

    s = std::make_shared<Session>(User::login(username, password));

    QJsonObject data;
    data["token"] = s->getToken();
    data["userId"] = s->getUser().getId();
    auto icon = s->getUser().getIcon();
    if(icon != "") {
      data["icon"] = icon;
    }

    // TODO: set other fields

    this->_manager->addClient(this->_clientId, s, username);

    this->_res = Message{Message::Type::USER, (int) Message::UserAction::LOGIN, Message::Status::RESPONSE, data};
    this->_has_resp = true;
  } catch(SQLNoElementSelectException&) {
    warn(ERROR_1);
    this->sendErrorMsg(ERROR_1);
  } catch(ClientLoginException&) {
    s->close();
    warn(ERROR_2);
    this->sendErrorMsg(ERROR_2);
  } catch(ClientMultipleLoginException&) {
    s->close();
    warn(ERROR_8);
    this->sendErrorMsg(ERROR_8);
  }
}

void ServerMessageProcessor::logout() {
  info("Logout query received");

  try {
    auto token = _m.getString("token");
    auto session = Session::get(token);
    session.close();

    this->_manager->clientDisconnected(this->_clientId);

    this->_has_resp = false;
  } catch(SQLNoElementSelectException&) {
    warn(ERROR_NO_SESSION);
    this->sendErrorMsg(ERROR_NO_SESSION);
  }
}

void ServerMessageProcessor::newUser() {
  info("NewUser query received");

  try {
    auto username = _m.getString("username");
    auto password = _m.getString("password");
    auto pswRepeat = _m.getString("pswRepeat");
    auto iconOpt = _m.getStringOpt("icon");
    if(password != pswRepeat) {
      debug(PASSWORD_NOT_MATCH);
      this->sendErrorMsg(PASSWORD_NOT_MATCH);
      return;
    }

    if(!check_pass(password)) {
      debug(PASSWORD_REQUIREMENTS);
      this->sendErrorMsg(PASSWORD_REQUIREMENTS);
      return;
    }

    try {
      auto u = User::registra(username, password, iconOpt);
    } catch(UserException& ex) {
      debug(ex.what());
      this->sendErrorMsg(ex.what());
      return;
    }

    auto s = std::make_shared<Session>(User::login(username, password));

    QJsonObject data;
    data["token"] = s->getToken();
    data["userId"] = s->getUser().getId();

    // TODO: set other fields

    this->_manager->addClient(this->_clientId, s, username);

    this->_res = Message{Message::Type::USER, (int) Message::UserAction::NEW, Message::Status::RESPONSE, data};
    this->_has_resp = true;
  } catch(SQLInsertException&) {
    error(ERROR_3);
    this->sendErrorMsg(ERROR_3);
  }
}

void ServerMessageProcessor::editUser() {
  info("EditUser query received");

  try {
    auto token = _m.getString("token");
    auto session = Session::get(token);

    auto user = session.getUser();

    auto old_password = _m.getStringOpt("oldPassword");
    auto password = _m.getStringOpt("password");
    auto pswRepeat = _m.getStringOpt("pswRepeat");

    if(old_password && password && pswRepeat) {
      if(*password != *pswRepeat) {
        debug(PASSWORD_NOT_MATCH);
        this->sendErrorMsg(PASSWORD_NOT_MATCH);
        return;
      }
      if(!check_pass(*password)) {
        debug(PASSWORD_REQUIREMENTS);
        this->sendErrorMsg(PASSWORD_REQUIREMENTS);
        return;
      }

      bool res = user.setPassword(*old_password, *password);
      if(!res) {
        debug(ERROR_4);
        this->sendErrorMsg(ERROR_4);
        return;
      }
    } else {
      debug("No pass change required");
    }


    auto icon = _m.getStringOpt("icon");
    if(icon) {
      try {
        user.setIcon(*icon);
      } catch(UserException& ex) {
        debug(ex.what());
        this->sendErrorMsg(ex.what());
        return;
      }
    } else {
      debug("No icon change required");
    }

    user.save();

    QJsonObject data;
    this->_res = Message{Message::Type::USER, (int) Message::UserAction::EDIT, Message::Status::RESPONSE, data};
    this->_has_resp = true;
  } catch(SQLNoElementUpdateException&) {
    error(ERROR_5);
    this->sendErrorMsg(ERROR_5);
  }
}

void ServerMessageProcessor::deleteUser() {
  info("DeleteUser query received");

  try {
    auto token = _m.getString("token");
    auto session = Session::get(token);

    auto user = session.getUser();

    this->_manager->removeClient(this->_clientId);

    session.close();
    user.remove();

    QJsonObject data;
    this->_res = Message{Message::Type::USER, (int) Message::UserAction::DELETE, Message::Status::RESPONSE, data};
    this->_has_resp = true;
  } catch(SQLNoElementDeleteException&) {
    error(ERROR_6);
    this->sendErrorMsg(ERROR_6);
  }
}

void ServerMessageProcessor::getUserIcon() {
  info("GetUserIcon query received");

  try {
    auto userId = _m.getInt("userId");
    auto user = Lazy<User>(userId);

    QJsonObject data;
    data["userId"] = userId;
    data["icon"] = user.getValue().getIcon();

    this->_res = Message{Message::Type::USER, (int) Message::UserAction::GET_USER_ICON, Message::Status::RESPONSE, data};
    this->_has_resp = true;
  } catch(SQLNoElementException&) {
    error(ERROR_6);
    this->sendErrorMsg(ERROR_6);
  }
}



void ServerMessageProcessor::newFile() {
  info("NewFile query received");

  auto token = _m.getString("token");
  auto session = Session::get(token);

  auto name = _m.getString("name");

  auto dirId = _m.getIntOpt("dirId");
  if(dirId && *dirId == 1) {
    dirId = std::nullopt;
  }

  FSElement_db dir = (dirId && (*dirId!=0)) ? FSElement_db::get(session, *dirId) : FSElement_db::root(session.getUserId());
  auto file = dir.mkfile(session, name);

  QJsonObject data;
  data["fileId"] = file.getId();

  this->_manager->getFile(this->_clientId, file.getId(), std::nullopt, true);

  this->_res = Message{Message::Type::FILE, (int) Message::FileAction::NEW, Message::Status::RESPONSE, data};
  this->_has_resp = true;
}

void ServerMessageProcessor::getFile() {
  info("GetFile query received");

  try {
    auto token = _m.getString("token");
    auto session = Session::get(token);

    auto file_db = FSElement_db::get(session, _m.getInt("fileId"));
    auto fileId = file_db.getPhysicalId();
    auto file = this->_manager->getFile(this->_clientId, fileId, file_db.getId());

    QJsonObject data;
    data["file"] = file.toJsonObject();
    data["charId"] = file_db.getCharId();
    data["commentId"] = file_db.getCommentId();

    this->_res = Message{Message::Type::FILE, (int) Message::FileAction::GET, Message::Status::RESPONSE, data};
    this->_has_resp = true;

    auto clients = this->_manager->getClientsInFile(fileId);
    for(auto &cl: clients) {
      if(cl == this->_clientId) {
        continue;
      }
      auto userId = this->_manager->getUserId(cl);

      QJsonObject data2;
      data2["fileId"] = FSElement_db::getIdForUser(session, _m.getInt("fileId"), userId);
      data2["userId"] = session.getUserId();
      data2["username"] = this->_manager->getUsername(this->_clientId);

      auto msg = Message{Message::Type::FILE_EDIT, (int) Message::FileEditAction::USER_CONNECTED, Message::Status::QUERY, data2};
      this->_manager->send_data(cl, msg.toQByteArray());
    }
  } catch(IllegalAccessException&) {
    warn(ACCESS_DENIED);
    this->sendErrorMsg(ACCESS_DENIED);
  }
}

void ServerMessageProcessor::closeFile() {
  info("CloseFile query received");

  auto token = _m.getString("token");
  auto session = Session::get(token);

  auto fileId = FSElement_db::get(session, _m.getInt("fileId")).getPhysicalId();

  this->_manager->closeFile(this->_clientId, fileId);

  auto clients = this->_manager->getClientsInFile(fileId);
  for(auto &cl: clients) {
    if(cl == this->_clientId) {
      continue;
    }
    auto userId = this->_manager->getUserId(cl);

    QJsonObject data;
    data["fileId"] = FSElement_db::getIdForUser(session, _m.getInt("fileId"), userId);
    data["userId"] = session.getUserId();

    auto msg = Message{Message::Type::FILE_EDIT, (int) Message::FileEditAction::USER_DISCONNECTED, Message::Status::QUERY, data};
    this->_manager->send_data(cl, msg.toQByteArray());
  }

  this->_has_resp = false;
}

void ServerMessageProcessor::editFile() {
  info("EditFile query received");

  auto token = _m.getString("token");
  auto session = Session::get(token);

  auto fileId = _m.getInt("fileId");
  auto file = FSElement_db::get(session, fileId);

  auto name = _m.getStringOpt("name");
  if(name) {
    file.rename(session, *name);
  } else {
    debug("No name change required");
  }

  QJsonObject data;

  this->_res = Message{Message::Type::FILE, (int) Message::FileAction::EDIT, Message::Status::RESPONSE, data};
  this->_has_resp = true;
}

void ServerMessageProcessor::deleteFile() {
  info("DeleteFile query received");

  auto token = _m.getString("token");
  auto session = Session::get(token);

  auto file = FSElement_db::get(session, _m.getInt("fileId"));
  auto fileId = file.getPhysicalId();

  this->_manager->closeFile(this->_clientId, fileId, true);

  file.remove(ServerMessageProcessor::delete_lambda);

  QJsonObject data;

  this->_res = Message{Message::Type::FILE, (int) Message::FileAction::DELETE, Message::Status::RESPONSE, data};
  this->_has_resp = true;
}

void ServerMessageProcessor::getLink() {
  info("GetLink query received");

  auto token = _m.getString("token");
  auto session = Session::get(token);

  auto file = FSElement_db::get(session, _m.getInt("fileId"));

  auto link = file.share(session);

  QJsonObject data;
  data["link"] = link.getToken();

  this->_res = Message{Message::Type::FILE, (int) Message::FileAction::GET_LINK, Message::Status::RESPONSE, data};
  this->_has_resp = true;
}

void ServerMessageProcessor::activateLink() {
  info("ActivateLink query received");

  try {
    auto token = _m.getString("token");
    auto session = Session::get(token);

    auto link_token = _m.getString("link");
    auto user_root = FSElement_db::root(session.getUserId());

    auto file = FSElement_db::link(session, link_token);
    file.mv(session, user_root, " (link)");

    auto fileId = file.getPhysicalId();

    QJsonObject data;
    data["element"] = file.getFSElement().toJsonObject();
    data["file"] = this->_manager->getFile(this->_clientId, fileId, file.getId(), true).toJsonObject();

    this->_res = Message{Message::Type::FILE, (int) Message::FileAction::ACTIVATE_LINK, Message::Status::RESPONSE, data};
    this->_has_resp = true;

    auto clients = this->_manager->getClientsInFile(fileId);
    for(auto &cl: clients) {
      if(cl == this->_clientId) {
        continue;
      }
      auto userId = this->_manager->getUserId(cl);

      QJsonObject data2;
      data2["fileId"] = FSElement_db::getIdForUser(session, file.getId(), userId);
      data2["userId"] = session.getUserId();
      data2["username"] = this->_manager->getUsername(this->_clientId);

      auto msg = Message{Message::Type::FILE_EDIT, (int) Message::FileEditAction::USER_CONNECTED, Message::Status::QUERY, data2};
      this->_manager->send_data(cl, msg.toQByteArray());
    }
  } catch(ShareException&) {
    warn(ERROR_7);
    this->sendErrorMsg(ERROR_7);
  } catch(DoubleLinkException&) {
    warn(ERROR_9);
    this->sendErrorMsg(ERROR_9);
  }
}



void ServerMessageProcessor::localInsert() {
  info("LocalInsert query received");

  auto token = _m.getString("token");
  auto session = Session::get(token);

  auto file_db = FSElement_db::get(session, _m.getInt("fileId"));
  auto fileId = file_db.getPhysicalId();

  auto symbols = _m.getArray("symbols");
  this->_manager->addSymbols(this->_clientId, fileId, symbols);

  file_db.addCharId(symbols.count());

  auto clients = this->_manager->getClientsInFile(fileId);
  for(auto &cl: clients) {
    if(cl == this->_clientId) {
      continue;
    }
    auto userId = this->_manager->getUserId(cl);

    QJsonObject data;
    data["fileId"] = FSElement_db::getIdForUser(session, _m.getInt("fileId"), userId);
    data["symbols"] = _m.getArray("symbols");
    data["userId"] = session.getUserId();

    auto msg = Message{Message::Type::FILE_EDIT, (int) Message::FileEditAction::REMOTE_INSERT, Message::Status::QUERY, data};
    this->_manager->send_data(cl, msg.toQByteArray());
  }

  this->_has_resp = false;
}

void ServerMessageProcessor::localDelete() {
  info("LocalDelete query received");

  auto token = _m.getString("token");
  auto session = Session::get(token);

  auto fileId = FSElement_db::get(session, _m.getInt("fileId")).getPhysicalId();

  auto ids = _m.getArray("ids");
  this->_manager->deleteSymbols(this->_clientId, fileId, ids);

  auto clients = this->_manager->getClientsInFile(fileId);
  for(auto &cl: clients) {
    if(cl == this->_clientId) {
      continue;
    }
    auto userId = this->_manager->getUserId(cl);

    QJsonObject data;
    data["fileId"] = FSElement_db::getIdForUser(session, _m.getInt("fileId"), userId);
    data["ids"] = ids;
    data["userId"] = session.getUserId();

    auto msg = Message{Message::Type::FILE_EDIT, (int) Message::FileEditAction::REMOTE_DELETE, Message::Status::QUERY, data};
    this->_manager->send_data(cl, msg.toQByteArray());
  }

  this->_has_resp = false;
}

void ServerMessageProcessor::localUpdate() {
  info("LocalUpdate query received");

  auto token = _m.getString("token");
  auto session = Session::get(token);

  auto fileId = FSElement_db::get(session, _m.getInt("fileId")).getPhysicalId();

  auto symbols = _m.getArray("symbols");
  this->_manager->updateSymbols(this->_clientId, fileId, symbols);

  auto clients = this->_manager->getClientsInFile(fileId);
  for(auto &cl: clients) {
    if(cl == this->_clientId) {
      continue;
    }
    auto userId = this->_manager->getUserId(cl);

    QJsonObject data;
    data["fileId"] = FSElement_db::getIdForUser(session, _m.getInt("fileId"), userId);
    data["symbols"] = _m.getArray("symbols");
    data["userId"] = session.getUserId();

    auto msg = Message{Message::Type::FILE_EDIT, (int) Message::FileEditAction::REMOTE_UPDATE, Message::Status::QUERY, data};
    this->_manager->send_data(cl, msg.toQByteArray());
  }

  this->_has_resp = false;
}

void ServerMessageProcessor::localMove() {
  info("LocalMove query received");

  auto token = _m.getString("token");
  auto session = Session::get(token);

  auto fileId = FSElement_db::get(session, _m.getInt("fileId")).getPhysicalId();

  auto clients = this->_manager->getClientsInFile(fileId);
  for(auto &cl: clients) {
    if(cl == this->_clientId) {
      continue;
    }
    auto userId = this->_manager->getUserId(cl);

    QJsonObject data;
    data["fileId"] = FSElement_db::getIdForUser(session, _m.getInt("fileId"), userId);
    data["userId"] = session.getUserId();
    data["symbolId"] = _m.getObject("symbolId");
    data["cursorPosition"] = _m.getInt("cursorPosition");

    auto msg = Message{Message::Type::FILE_EDIT, (int) Message::FileEditAction::REMOTE_MOVE, Message::Status::QUERY, data};
    this->_manager->send_data(cl, msg.toQByteArray());
  }

  this->_has_resp = false;
}



void ServerMessageProcessor::newComment() {
  info("NewComment query received");

  auto token = _m.getString("token");
  auto session = Session::get(token);

  auto file_db = FSElement_db::get(session, _m.getInt("fileId"));
  auto fileId = file_db.getPhysicalId();

  auto comment = _m.getObject("comment");
  this->_manager->addComment(this->_clientId, fileId, comment);

  file_db.incCommentId();

  auto clients = this->_manager->getClientsInFile(fileId);
  for(auto &cl: clients) {
    if(cl == this->_clientId) {
      continue;
    }
    auto userId = this->_manager->getUserId(cl);

    QJsonObject data;
    data["fileId"] = FSElement_db::getIdForUser(session, _m.getInt("fileId"), userId);
    data["comment"] = comment;
    data["userId"] = session.getUserId();

    auto msg = Message{Message::Type::COMMENT, (int) Message::CommentAction::COMMENT_REMOTE_INSERT, Message::Status::QUERY, data};
    this->_manager->send_data(cl, msg.toQByteArray());
  }

  this->_has_resp = false;
}

void ServerMessageProcessor::updateComment() {
  info("UpdateComment query received");

  auto token = _m.getString("token");
  auto session = Session::get(token);

  auto file_db = FSElement_db::get(session, _m.getInt("fileId"));
  auto fileId = file_db.getPhysicalId();

  auto comment = _m.getObject("comment");
  this->_manager->updateComment(this->_clientId, fileId, session, comment);

  file_db.incCommentId();

  auto clients = this->_manager->getClientsInFile(fileId);
  for(auto &cl: clients) {
    if(cl == this->_clientId) {
      continue;
    }
    auto userId = this->_manager->getUserId(cl);

    QJsonObject data;
    data["fileId"] = FSElement_db::getIdForUser(session, _m.getInt("fileId"), userId);
    data["comment"] = comment;
    data["userId"] = session.getUserId();

    auto msg = Message{Message::Type::COMMENT, (int) Message::CommentAction::COMMENT_REMOTE_UPDATE, Message::Status::QUERY, data};
    this->_manager->send_data(cl, msg.toQByteArray());
  }

  this->_has_resp = false;
}

void ServerMessageProcessor::deleteComment() {
  info("DeleteComment query received");

  auto token = _m.getString("token");
  auto session = Session::get(token);

  auto file_db = FSElement_db::get(session, _m.getInt("fileId"));
  auto fileId = file_db.getPhysicalId();

  auto comment = _m.getObject("comment");
  this->_manager->deleteComment(this->_clientId, fileId, session, comment);

  auto clients = this->_manager->getClientsInFile(fileId);
  for(auto &cl: clients) {
    if(cl == this->_clientId) {
      continue;
    }
    auto userId = this->_manager->getUserId(cl);

    QJsonObject data;
    data["fileId"] = FSElement_db::getIdForUser(session, _m.getInt("fileId"), userId);
    data["comment"] = comment;
    data["userId"] = session.getUserId();

    auto msg = Message{Message::Type::COMMENT, (int) Message::CommentAction::COMMENT_REMOTE_DELETE, Message::Status::QUERY, data};
    this->_manager->send_data(cl, msg.toQByteArray());
  }

  this->_has_resp = false;
}



void ServerMessageProcessor::newDir() {
  info("NewDir query received");

  auto token = _m.getString("token");
  auto session = Session::get(token);

  auto name = _m.getString("name");
  auto parent_id = _m.getIntOpt("parentId");
  if(parent_id && *parent_id == 1) {
    parent_id = std::nullopt;
  }

  auto dir = (parent_id && (*parent_id!=0)) ? FSElement_db::get(session, *parent_id) : FSElement_db::root(session.getUserId());
  auto new_dir = dir.mkdir(session, name);

  QJsonObject data;
  data["dirId"] = new_dir.getId();

  this->_res = Message{Message::Type::FILESYSTEM, (int) Message::FileSystemAction::NEW_DIR, Message::Status::RESPONSE, data};
  this->_has_resp = true;
}

void ServerMessageProcessor::editDir() {
  info("EditDir query received");

  auto token = _m.getString("token");
  auto session = Session::get(token);

  auto dir_id = _m.getInt("dirId");
  auto dir = FSElement_db::get(session, dir_id);

  auto name = _m.getStringOpt("name");
  if(name) {
    dir.rename(session, *name);
  } else {
    debug("No name change required");
  }

  auto parent_id = _m.getIntOpt("parentId");
  if(parent_id) {
    dir.mv(session, *parent_id);
  } else {
    debug("No parentId change required");
  }

  QJsonObject data;

  this->_res = Message{Message::Type::FILESYSTEM, (int) Message::FileSystemAction::EDIT_DIR, Message::Status::RESPONSE, data};
  this->_has_resp = true;
}

void ServerMessageProcessor::deleteDir() {
  info("DeleteDir query received");

  auto token = _m.getString("token");
  auto session = Session::get(token);

  auto dir_id = _m.getInt("dirId");
  auto dir = FSElement_db::get(session, dir_id);

  dir.remove(ServerMessageProcessor::delete_lambda);

  QJsonObject data;

  this->_res = Message{Message::Type::FILESYSTEM, (int) Message::FileSystemAction::DELETE_DIR, Message::Status::RESPONSE, data};
  this->_has_resp = true;
}

void ServerMessageProcessor::getDir() {
  info("GetDir query received");

  auto token = _m.getString("token");
  auto session = Session::get(token);

  auto dir_id = _m.getIntOpt("dirId");
  if(dir_id && *dir_id == 1) {
    dir_id = std::nullopt;
  }

  auto dir = (dir_id && (*dir_id!=0)) ? FSElement_db::get(session, *dir_id) : FSElement_db::root(session.getUserId());

  QJsonArray arr;
  for(auto& f: dir.ls(session)) {
    auto obj = f->getFSElement().toJsonObject();
    arr.append(obj);
  }

  QJsonObject data;
  data["name"] = dir.getName();
  data["parent"] = dir.getParentId();
  data["elements"] = arr;

  this->_res = Message{Message::Type::FILESYSTEM, (int) Message::FileSystemAction::GET_DIR, Message::Status::RESPONSE, data};
  this->_has_resp = true;
}

void ServerMessageProcessor::moveFile() {
  info("MoveFile query received");

  auto token = _m.getString("token");
  auto session = Session::get(token);

  auto file_id = _m.getInt("fileId");

  auto dir_id = _m.getIntOpt("dirId");
  if(dir_id && *dir_id == 1) {
    dir_id = std::nullopt;
  }

  auto file = FSElement_db::get(session, file_id);
  auto dir = (dir_id && (*dir_id!=0)) ? FSElement_db::get(session, *dir_id) : FSElement_db::root(session.getUserId());

  file.mv(session, dir);

  QJsonObject data;

  this->_res = Message{Message::Type::FILESYSTEM, (int) Message::FileSystemAction::MOVE_FILE, Message::Status::RESPONSE, data};
  this->_has_resp = true;
}

void ServerMessageProcessor::getPath() {
  info("GetPath query received");

  auto token = _m.getString("token");
  auto session = Session::get(token);

  auto element_id = _m.getInt("elementId");

  auto element = (element_id!=1) ? FSElement_db::get(session, element_id) : FSElement_db::root(session.getUserId());
  auto elements = element.getPathElements(session);

  QJsonArray arr;
  for(auto& f: elements) {
    auto obj = f.getFSElement().toJsonObject();
    arr.append(obj);
  }

  QJsonObject data;
  data["elements"] = arr;

  this->_res = Message{Message::Type::FILESYSTEM, (int) Message::FileSystemAction::GET_PATH, Message::Status::RESPONSE, data};
  this->_has_resp = true;
}

void ServerMessageProcessor::getAllDirs() {
  info("GetAllDirs query received");

  auto token = _m.getString("token");
  auto session = Session::get(token);

  auto root = FSElement_db::root(session.getUserId());
  auto elements = root.recursive_ls(session);

  QJsonArray arr;
  for(auto& f: elements) {
    auto pair = f.pwd(session);
    QJsonObject obj;
    obj["path"] = pair.first;
    obj["id"] = pair.second;
    debug(pair.first);
    arr.append(obj);
  }

  QJsonObject data;
  data["elements"] = arr;

  this->_res = Message{Message::Type::FILESYSTEM, (int) Message::FileSystemAction::GET_ALL_DIRS, Message::Status::RESPONSE, data};
  this->_has_resp = true;
}

void ServerMessageProcessor::delete_lambda(int link_id, int owner_id) {
  debug("NOTIFY: removed link_id "+QString::number(link_id)+" of user "+QString::number(owner_id));

  QJsonObject data;
  data["fileId"] = link_id;

  auto msg = Message{Message::Type::FILE, (int) Message::FileAction::FILE_DELETED, Message::Status::QUERY, data};
  auto byte_data = msg.toQByteArray();

  auto manager = MessageManager::get();
  auto client = manager->getClient(owner_id);
  if(client) {
    manager->send_data(*client, byte_data);
  }
}
