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

    auto s = std::make_shared<Session>(User::login(username, password));

    QJsonObject data;
    data["token"] = s->getToken();
    data["userId"] = s->getUser().getId();

    // TODO: set other fields

    this->_manager->addClient(this->_clientId, s, username);

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

    this->_manager->clientDisconnected(this->_clientId);

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
			if(!User::check_email(email)) {
				// TODO
				throw 1;
			}
    } catch(MessageDataException ex) {
      // TODO: vedi come fare per email vuota
    }
    auto nickname = _m.getString("username");
    auto password = _m.getString("password");
		auto pswRepeat = _m.getString("pswRepeat");
		if(password != pswRepeat) {
			debug("Password and PswRepeat does not match");
			// TODO
			throw 1;
		}

		if(!User::check_pass(password)) {
			// TODO
			throw 1;
		}

    auto u = User::registra(nickname, email, password);
    auto s = std::make_shared<Session>(User::login(nickname, password));

    QJsonObject data;
    data["token"] = s->getToken();
    data["userId"] = s->getUser().getId();

    // TODO: set other fields

    this->_manager->addClient(this->_clientId, s, nickname);

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
				if(!User::check_pass(password)) {
					// TODO
					throw 1;
				}
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
  // TODO: l'utente può solo cancellarsi da solo, quindi devo anche farne il logout
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

	this->_manager->getFile(this->_clientId, file.getId());

  this->_res = Message{Message::Type::FILE, (int) Message::FileAction::NEW, Message::Status::RESPONSE, data};
  this->_has_resp = true;
}

void ServerMessageProcessor::getFile() {
  info("GetFile query received");

  auto token = _m.getString("token");
  auto session = Session::get(token);

  auto fileId = FSElement_db::get(session, _m.getInt("fileId")).getPhisicalId();
  auto file = this->_manager->getFile(this->_clientId, fileId);

  QJsonObject data;
  data["file"] = file.toJsonObject();
  // TODO
  data["charId"] = 0;

  this->_res = Message{Message::Type::FILE, (int) Message::FileAction::GET, Message::Status::RESPONSE, data};
  this->_has_resp = true;

  QJsonObject data2;
  data2["fileId"] = _m.getInt("fileId");
  data2["clientId"] = (int) this->_clientId;
  data2["username"] = this->_manager->getUsername(this->_clientId);

  auto msg = Message{Message::Type::FILE_EDIT, (int) Message::FileEditAction::USER_CONNECTED, Message::Status::QUERY, data2};
  this->_manager->sendToAll(this->_clientId, fileId, msg.toQByteArray());
}

void ServerMessageProcessor::closeFile() {
  info("CloseFile query received");

  auto token = _m.getString("token");
  auto session = Session::get(token);

  auto fileId = FSElement_db::get(session, _m.getInt("fileId")).getPhisicalId();

  this->_manager->closeFile(this->_clientId, fileId);

  auto clients = this->_manager->getClientsInFile(fileId);
  for(auto &cl: clients) {
    if(cl == this->_clientId) {
      continue;
    }
    auto userId = this->_manager->getUserId(cl);

    QJsonObject data;
    data["fileId"] = FSElement_db::getIdForUser(session, _m.getInt("fileId"), userId);
    data["clientId"] = (int) this->_clientId;

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

  auto file = FSElement_db::get(session, _m.getInt("fileId"));

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

  auto token = _m.getString("token");
  auto session = Session::get(token);

  auto link_token = _m.getString("link");
  auto user_root = FSElement_db::root(session.getUserId());

  auto file = FSElement_db::link(session, link_token);
  file.mv(session, user_root);

	this->_manager->getFile(this->_clientId, file.getId());

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
  info("LocalInsert query received");

  auto token = _m.getString("token");
  auto session = Session::get(token);

  auto fileId = FSElement_db::get(session, _m.getInt("fileId")).getPhisicalId();

  auto symbols = _m.getArray("symbols");
  for(const auto &symb: symbols) {
    this->_manager->addSymbol(this->_clientId, fileId, Symbol::fromJsonObject(symb.toObject()));
  }

  auto clients = this->_manager->getClientsInFile(fileId);
  for(auto &cl: clients) {
    if(cl == this->_clientId) {
      continue;
    }
    auto userId = this->_manager->getUserId(cl);

    QJsonObject data;
    data["fileId"] = FSElement_db::getIdForUser(session, _m.getInt("fileId"), userId);
    data["symbols"] = _m.getArray("symbols");
    data["clientId"] = session.getUserId();

    auto msg = Message{Message::Type::FILE_EDIT, (int) Message::FileEditAction::REMOTE_INSERT, Message::Status::QUERY, data};
    this->_manager->send_data(cl, msg.toQByteArray());
  }

  this->_has_resp = false;
}

void ServerMessageProcessor::localDelete() {
  info("LocalDelete query received");

  auto token = _m.getString("token");
  auto session = Session::get(token);

  auto fileId = FSElement_db::get(session, _m.getInt("fileId")).getPhisicalId();

  auto ids = _m.getArray("ids");
  for(const auto &id: ids) {
    this->_manager->deleteSymbol(this->_clientId, fileId, SymbolId::fromJsonObject(id.toObject()));
  }

  auto clients = this->_manager->getClientsInFile(fileId);
  for(auto &cl: clients) {
    if(cl == this->_clientId) {
      continue;
    }
    auto userId = this->_manager->getUserId(cl);

    QJsonObject data;
    data["fileId"] = FSElement_db::getIdForUser(session, _m.getInt("fileId"), userId);
    data["ids"] = _m.getArray("ids");
    data["clientId"] = session.getUserId();

    auto msg = Message{Message::Type::FILE_EDIT, (int) Message::FileEditAction::REMOTE_DELETE, Message::Status::QUERY, data};
    this->_manager->send_data(cl, msg.toQByteArray());
  }

  this->_has_resp = false;
}

void ServerMessageProcessor::localUpdate() {
  info("LocalUpdate query received");

  auto token = _m.getString("token");
  auto session = Session::get(token);

  auto fileId = FSElement_db::get(session, _m.getInt("fileId")).getPhisicalId();

  auto symbols = _m.getArray("symbols");
  for(const auto &symb: symbols) {
    auto s = Symbol::fromJsonObject(symb.toObject());
    auto to_mod = this->_manager->getSymbol(this->_clientId, fileId, s.getSymbolId());
    to_mod.update(s);
  }

  auto clients = this->_manager->getClientsInFile(fileId);
  for(auto &cl: clients) {
    if(cl == this->_clientId) {
      continue;
    }
    auto userId = this->_manager->getUserId(cl);

    QJsonObject data;
    data["fileId"] = FSElement_db::getIdForUser(session, _m.getInt("fileId"), userId);
    data["symbols"] = _m.getArray("symbols");
    data["clientId"] = session.getUserId();

    auto msg = Message{Message::Type::FILE_EDIT, (int) Message::FileEditAction::REMOTE_UPDATE, Message::Status::QUERY, data};
    this->_manager->send_data(cl, msg.toQByteArray());
  }

  this->_has_resp = false;
}

void ServerMessageProcessor::localMove() {
  info("LocalMove query received");

  auto token = _m.getString("token");
  auto session = Session::get(token);

  auto fileId = FSElement_db::get(session, _m.getInt("fileId")).getPhisicalId();

  auto symbolId = SymbolId::fromJsonObject(_m.getObject("symbolId"));
  auto cursorPosition = _m.getInt("cursorPosition");
  // TODO: sposta il cursore nel file

  auto clients = this->_manager->getClientsInFile(fileId);
  for(auto &cl: clients) {
    if(cl == this->_clientId) {
      continue;
    }
    auto userId = this->_manager->getUserId(cl);

    QJsonObject data;
    data["fileId"] = FSElement_db::getIdForUser(session, _m.getInt("fileId"), userId);
    data["clientId"] = (int) this->_clientId;
    data["symbolId"] = _m.getObject("symbolId");
    data["cursorPosition"] = _m.getInt("cursorPosition");

    auto msg = Message{Message::Type::FILE_EDIT, (int) Message::FileEditAction::REMOTE_MOVE, Message::Status::QUERY, data};
    this->_manager->send_data(cl, msg.toQByteArray());
  }

  this->_has_resp = false;
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

  dir.remove(ServerMessageProcessor::delete_lambda);

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

void ServerMessageProcessor::delete_lambda(int link_id, int owner_id) {
  debug("NOTIFY: removed link_id "+QString::number(link_id)+" of user "+QString::number(owner_id));

  QJsonObject data;
  data["fileId"] = link_id;

  auto msg = Message{Message::Type::FILE, (int) Message::FileAction::FILE_DELETED, Message::Status::QUERY, data};
  auto byte_data = msg.toQByteArray();

  auto manager = MessageManager::get();
  auto clients = manager->getClients(owner_id);
  for(auto &cl: clients) {
    manager->send_data(cl, byte_data);
  }
}
