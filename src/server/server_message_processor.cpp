#include "server_message_processor.h"

#include "user.h"

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

void ServerMessageProcessor::newUser() {
  info("NewUser query received");

  try {
    QString nickname = "";
    try {
      nickname = _m.getString("nickname");
    } catch(MessageDataException ex) {
      nickname = _m.getString("email");
    }
    auto email = _m.getString("email");
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

}

void ServerMessageProcessor::deleteUser() {

}
