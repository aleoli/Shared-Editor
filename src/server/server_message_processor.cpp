#include "server_message_processor.h"

#include "user.h"
#include "session.h"

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
  //TODO switch statement
  std::cout << "ciao server" << std::endl;
}

void ServerMessageProcessor::process_file_edit() {
  //TODO switch statement
  std::cout << "ciao server" << std::endl;
}

void ServerMessageProcessor::process_filesystem() {
  //TODO switch statement
  std::cout << "ciao server" << std::endl;
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
