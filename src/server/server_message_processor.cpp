#include "server_message_processor.h"

#include "user.h"
#include "session.h"
#include "FSElement_db.h"

ServerMessageProcessor::ServerMessageProcessor(const Message &m, quint64 clientId)
    : MessageProcessor(m), _clientId(clientId), _to_all(false) {
  _manager = MessageManager::get();
  this->process_message();
}

bool ServerMessageProcessor::shouldSendToAll() const {
  return this->_to_all;
}

void ServerMessageProcessor::process_error() {
  // TODO: non so cosa farmene dell'errore lato server, disconnetto il client?!?
  info("ERROR message received");
  auto reason = _m.getString("reason");
  debug(reason);
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

    default:
      disconnect("Ricevuto messaggio con azione non valida");
  }
}

void ServerMessageProcessor::disconnect(QString why) {
  error(why);
  // TODO
  // emit _manager->connection_error();
}





// ------ handlers ------

void ServerMessageProcessor::login() {
  info("Login query received");

  try {
    auto username = _m.getString("username");
    auto password = _m.getString("password");

    auto s = User::login(username, password);

    QJsonObject data;
    data["token"] = s.getToken();
    data["userId"] = s.getUser().getId();

    // TODO: set other fields

    this->_res = Message{Message::Type::USER, (int) Message::UserAction::LOGIN, Message::Status::RESPONSE, data};
    this->_has_resp = true;
  } catch(...) {
    // TODO
    error("EXCEPTION: TODO");
  }
}

void ServerMessageProcessor::logout() {
  info("Logout query received");

  try {
    auto token = _m.getString("token");
    auto session = Session::get(token);
    session.close();

    this->_has_resp = false;
  } catch(...) {
    // TODO
    error("EXCEPTION: TODO");
  }
}

void ServerMessageProcessor::newUser() {
  info("NewUser query received");

  try {
    QString email = "";
    try {
      email = _m.getString("email");
    } catch(MessageDataException ex) {
      // TODO: vedi come fare per email vuota
    }
    auto nickname = _m.getString("username");
    auto password = _m.getString("password");

    auto u = User::registra(nickname, email, password);
    auto s = User::login(nickname, password);

    QJsonObject data;
    data["token"] = s.getToken();
    data["userId"] = s.getUser().getId();

    // TODO: set other fields

    this->_res = Message{Message::Type::USER, (int) Message::UserAction::NEW, Message::Status::RESPONSE, data};
    this->_has_resp = true;
  } catch(...) {
    // TODO
    error("EXCEPTION: TODO");
  }
}

void ServerMessageProcessor::editUser() {
  info("EditUser query received");

  try {
    auto token = _m.getString("token");
    auto session = Session::get(token);

    auto user = session.getUser();

    try {
      auto old_password = _m.getString("oldPassword");
      auto password = _m.getString("password");
      if(password == _m.getString("pswRepeat")) {
        bool res = user.setPassword(old_password, password);
        if(!res) {
          // TODO: va mandato un messaggio di errore
        }
      }
    } catch(MessageDataException ex) {
      debug("No pass change required");
    }


    try {
      auto nickname = _m.getString("nickname");
      user.setNickname(nickname);
    } catch(MessageDataException ex) {
      debug("No nickname change required");
    }


    try {
      auto icon = _m.getString("icon");
      user.setIcon(icon);
    } catch(MessageDataException ex) {
      debug("No icon change required");
    }

    user.save();

    QJsonObject data;
    this->_res = Message{Message::Type::USER, (int) Message::UserAction::EDIT, Message::Status::RESPONSE, data};
    this->_has_resp = true;
  } catch(...) {
    // TODO
    error("EXCEPTION: TODO");
  }
}

void ServerMessageProcessor::deleteUser() {
  info("DeleteUser query received");

  try {
    auto token = _m.getString("token");
    auto session = Session::get(token);

    auto user = session.getUser();

    user.remove();

    QJsonObject data;
    this->_res = Message{Message::Type::USER, (int) Message::UserAction::DELETE, Message::Status::RESPONSE, data};
    this->_has_resp = true;
  } catch(...) {
    // TODO
    error("EXCEPTION: TODO");
  }
}



void ServerMessageProcessor::newFile() {
  info("NewFile query received");

  auto token = _m.getString("token");
  auto session = Session::get(token);

  auto name = _m.getString("name");

  int dirId = 0;
  try {
    dirId = _m.getInt("dirId");
  } catch(MessageDataException ex) {
    // use root dir
  }

  FSElement_db dir = dirId == 0 ? FSElement_db::root(session.getUserId()) : FSElement_db::get(session, dirId);
  auto file = dir.mkfile(session, name);

  QJsonObject data;
  data["fileId"] = file.getId();

  this->_res = Message{Message::Type::FILE, (int) Message::FileAction::NEW, Message::Status::RESPONSE, data};
  this->_has_resp = true;
}

void ServerMessageProcessor::getFile() {
  info("GetFile query received");

  auto token = _m.getString("token");
  auto session = Session::get(token);

  auto fileId = _m.getInt("fileId");
  auto file = FSElement_db::get(session, fileId);

  QJsonObject data;
  data["file"] = file.getFSElement().toJsonObject();
  // TODO
  data["charId"] = 0;

  this->_res = Message{Message::Type::FILE, (int) Message::FileAction::GET, Message::Status::RESPONSE, data};
  this->_has_resp = true;
}

void ServerMessageProcessor::closeFile() {
  info("CloseFile query received");

  auto token = _m.getString("token");
  auto session = Session::get(token);

  auto fileId = _m.getInt("fileId");

  // TODO

  this->_has_resp = false;
}

void ServerMessageProcessor::editFile() {
  info("EditFile query received");

  auto token = _m.getString("token");
  auto session = Session::get(token);

  auto fileId = _m.getInt("fileId");
  auto file = FSElement_db::get(session, fileId);

  try {
    auto name = _m.getString("name");
    file.rename(session, name);
  } catch(MessageDataException ex) {
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

  auto fileId = _m.getInt("fileId");
  auto file = FSElement_db::get(session, fileId);

  file.remove([](int link_id, int owner_id) -> void {
    // TODO
    debug("NOTIFY: removed link_id "+QString::number(link_id)+" of user "+QString::number(owner_id));
  });

  QJsonObject data;

  this->_res = Message{Message::Type::FILE, (int) Message::FileAction::DELETE, Message::Status::RESPONSE, data};
  this->_has_resp = true;
}

void ServerMessageProcessor::getLink() {
  info("GetLink query received");

  auto token = _m.getString("token");
  auto session = Session::get(token);

  auto fileId = _m.getInt("fileId");
  auto file = FSElement_db::get(session, fileId);

  auto link = file.share(session);

  QJsonObject data;
  data["link"] = link.getToken();

  this->_res = Message{Message::Type::FILE, (int) Message::FileAction::GET_LINK, Message::Status::RESPONSE, data};
  this->_has_resp = true;
}

void ServerMessageProcessor::activateLink() {
  info("ActivateLink query received");

  auto token = _m.getString("token");
  auto session = Session::get(token);

  auto link_token = _m.getString("link");
  auto user_root = FSElement_db::root(session.getUserId());

  auto file = FSElement_db::link(session, link_token);
  file.mv(session, user_root);

  QJsonObject data;
  QJsonObject element;
  element["fileId"] = file.getId();
  element["name"] = file.getName();
  data["element"] = element;
  data["file"] = file.getFSElement().toJsonObject();

  this->_res = Message{Message::Type::FILE, (int) Message::FileAction::ACTIVATE_LINK, Message::Status::RESPONSE, data};
  this->_has_resp = true;
}



void ServerMessageProcessor::localInsert() {
  // TODO
}

void ServerMessageProcessor::localDelete() {
  // TODO
}

void ServerMessageProcessor::localUpdate() {
  // TODO
}

void ServerMessageProcessor::localMove() {
  // TODO
}



void ServerMessageProcessor::newDir() {
  info("NewDir query received");

  auto token = _m.getString("token");
  auto session = Session::get(token);

  auto name = _m.getString("name");
  int parent_id = 0;
  try {
    parent_id = _m.getInt("parentId");
  } catch(MessageDataException ex) {
    // use root directory
  }

  auto dir = parent_id == 0 ? FSElement_db::root(session.getUserId()) : FSElement_db::get(session, parent_id);
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

  try {
    auto name = _m.getString("name");
    dir.rename(session, name);
  } catch(MessageDataException ex) {
    debug("No name change required");
  }

  try {
    auto parent_id = _m.getInt("parentId");
    dir.mv(session, parent_id);
  } catch(MessageDataException ex) {
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

  dir.remove([](int link_id, int owner_id) -> void {
    // TODO
    debug("NOTIFY: removed link_id "+QString::number(link_id)+" of user "+QString::number(owner_id));
  });

  QJsonObject data;

  this->_res = Message{Message::Type::FILESYSTEM, (int) Message::FileSystemAction::DELETE_DIR, Message::Status::RESPONSE, data};
  this->_has_resp = true;
}

void ServerMessageProcessor::getDir() {
  info("GetDir query received");

  auto token = _m.getString("token");
  auto session = Session::get(token);

  auto dir_id = 0;
  try {
    dir_id = _m.getInt("dirId");
  } catch(MessageDataException ex) {
    // use root directory
  }

  auto dir = dir_id == 0 ? FSElement_db::root(session.getUserId()) : FSElement_db::get(session, dir_id);

  QJsonArray arr;
  for(auto& f: dir.ls(session)) {
    auto obj = f->getFSElement().toJsonObject();
    arr.append(obj);
  }

  QJsonObject data;
  data["elements"] = arr;

  this->_res = Message{Message::Type::FILESYSTEM, (int) Message::FileSystemAction::GET_DIR, Message::Status::RESPONSE, data};
  this->_has_resp = true;
}

void ServerMessageProcessor::moveFile() {
  info("MoveFile query received");

  auto token = _m.getString("token");
  auto session = Session::get(token);

  auto file_id = _m.getInt("fileId");

  auto dir_id = 0;
  try {
    dir_id = _m.getInt("dirId");
  } catch(MessageDataException ex) {
    // use root directory
  }

  auto file = FSElement_db::get(session, file_id);
  auto dir = dir_id == 0 ? FSElement_db::root(session.getUserId()) : FSElement_db::get(session, dir_id);

  file.mv(session, dir);

  QJsonObject data;

  this->_res = Message{Message::Type::FILESYSTEM, (int) Message::FileSystemAction::MOVE_FILE, Message::Status::RESPONSE, data};
  this->_has_resp = true;
}
