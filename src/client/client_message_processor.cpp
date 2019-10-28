#include "client_message_processor.h"

#include "utils.h"
#include "exceptions.h"

using namespace se_exceptions;

ClientMessageProcessor::ClientMessageProcessor(const Message &m): MessageProcessor(m) {
  _manager = MessageManager::get();
  this->process_message();
}

void ClientMessageProcessor::process_error() {
  info("ERROR message received");

  auto reason = _m.getString("reason");

  debug(reason);

  emit _manager->errorResponse(reason);
}

void ClientMessageProcessor::process_user() {
  auto action = static_cast<Message::UserAction>(_m.getAction());
  bool isResponse = _m.getStatus() == Message::Status::RESPONSE;

  switch(action) {
    case Message::UserAction::LOGIN:
      if(!isResponse)
        disconnect("Ricevuto messaggio con status non valido");
      else
        login();
      break;

    case Message::UserAction::NEW:
      if(!isResponse)
        disconnect("Ricevuto messaggio con status non valido");
      else
        newUser();
      break;

    case Message::UserAction::EDIT:
      if(!isResponse)
        disconnect("Ricevuto messaggio con status non valido");
      else
        editUser();
      break;

    case Message::UserAction::DELETE:
      if(!isResponse)
        disconnect("Ricevuto messaggio con status non valido");
      else
        deleteUser();
      break;

    default:
      disconnect("Ricevuto messaggio con azione non valida");
  }
}

void ClientMessageProcessor::process_file() {
  auto action = static_cast<Message::FileAction>(_m.getAction());
  bool isResponse = _m.getStatus() == Message::Status::RESPONSE;

  switch(action) {
    case Message::FileAction::NEW:
      if(!isResponse)
        disconnect("Ricevuto messaggio con status non valido");
      else
        newFile();
      break;

    case Message::FileAction::GET:
      if(!isResponse)
        disconnect("Ricevuto messaggio con status non valido");
      else
        getFile();
      break;

    case Message::FileAction::EDIT:
      if(!isResponse)
        disconnect("Ricevuto messaggio con status non valido");
      else
        editFile();
      break;

    case Message::FileAction::DELETE:
      if(!isResponse)
        disconnect("Ricevuto messaggio con status non valido");
      else
        deleteFile();
      break;

    case Message::FileAction::FILE_DELETED:
      if(isResponse)
        disconnect("Ricevuto messaggio con status non valido");
      else
        fileDeleted();
      break;

    case Message::FileAction::GET_LINK:
      if(!isResponse)
        disconnect("Ricevuto messaggio con status non valido");
      else
        getLink();
      break;

    case Message::FileAction::ACTIVATE_LINK:
      if(!isResponse)
        disconnect("Ricevuto messaggio con status non valido");
      else
        activateLink();
      break;

    default:
      disconnect("Ricevuto messaggio con azione non valida");
  }
}

void ClientMessageProcessor::process_file_edit() {
  auto action = static_cast<Message::FileEditAction>(_m.getAction());
  bool isResponse = _m.getStatus() == Message::Status::RESPONSE;

  switch(action) {
    case Message::FileEditAction::REMOTE_INSERT:
      if(isResponse)
        disconnect("Ricevuto messaggio con status non valido");
      else
        remoteInsert();
      break;

    case Message::FileEditAction::REMOTE_DELETE:
      if(isResponse)
        disconnect("Ricevuto messaggio con status non valido");
      else
        remoteDelete();
      break;

    case Message::FileEditAction::REMOTE_UPDATE:
      if(isResponse)
        disconnect("Ricevuto messaggio con status non valido");
      else
        remoteUpdate();
      break;

    case Message::FileEditAction::USER_CONNECTED:
      if(isResponse)
        disconnect("Ricevuto messaggio con status non valido");
      else
        userConnected();
      break;

    case Message::FileEditAction::USER_DISCONNECTED:
      if(isResponse)
        disconnect("Ricevuto messaggio con status non valido");
      else
        userDisconnected();
      break;

    case Message::FileEditAction::REMOTE_MOVE:
      if(isResponse)
        disconnect("Ricevuto messaggio con status non valido");
      else
        remoteMove();
      break;

    default:
      disconnect("Ricevuto messaggio con azione non valida");
  }
}

void ClientMessageProcessor::process_filesystem() {
  auto action = static_cast<Message::FileSystemAction>(_m.getAction());
  bool isResponse = _m.getStatus() == Message::Status::RESPONSE;

  switch(action) {
    case Message::FileSystemAction::NEW_DIR:
      if(!isResponse)
        disconnect("Ricevuto messaggio con status non valido");
      else
        newDir();
      break;

    case Message::FileSystemAction::EDIT_DIR:
      if(!isResponse)
        disconnect("Ricevuto messaggio con status non valido");
      else
        editDir();
      break;

    case Message::FileSystemAction::DELETE_DIR:
      if(!isResponse)
        disconnect("Ricevuto messaggio con status non valido");
      else
        deleteDir();
      break;

    case Message::FileSystemAction::GET_DIR:
      if(!isResponse)
        disconnect("Ricevuto messaggio con status non valido");
      else
        getDir();
      break;

    case Message::FileSystemAction::MOVE_FILE:
      if(!isResponse)
        disconnect("Ricevuto messaggio con status non valido");
      else
        moveFile();
      break;

    default:
      disconnect("Ricevuto messaggio con azione non valida");
  }
}

void ClientMessageProcessor::disconnect(QString why) {
  error(why);
  emit _manager->connection_error();
}

void ClientMessageProcessor::login() {
  info("Login response received");
  QString nickname = nullptr, icon = nullptr;

  auto token = _m.getString("token");

  try {
    nickname = _m.getString("nickname");
  }
  catch(MessageDataException e) {}

  try {
    icon = _m.getString("icon");
  }
  catch(MessageDataException e) {}

  emit _manager->loginResponse(token, nickname, icon);
}

void ClientMessageProcessor::newUser() {
  info("USER::NEW response received");

  auto token = _m.getString("token");

  emit _manager->newUserResponse(token);
}

void ClientMessageProcessor::editUser() {
  info("USER::EDIT response received");

  emit _manager->editUserResponse();
}

void ClientMessageProcessor::deleteUser() {
  info("USER::DELETE response received");

  emit _manager->deleteUserResponse();
}

void ClientMessageProcessor::newFile() {
  info("FILE::NEW response received");

  auto fileId = _m.getInt("fileId");

  emit _manager->newFileResponse(fileId);
}

void ClientMessageProcessor::getFile() {
  info("FILE::GET response received");

  auto file = _m.getObject("file");
  auto charId = _m.getInt("charId");

  emit _manager->getFileResponse(File::fromJsonObject(file), charId);
}

void ClientMessageProcessor::editFile() {
  info("FILE::EDIT response received");

  emit _manager->editFileResponse();
}

void ClientMessageProcessor::deleteFile() {
  info("FILE::DELETE response received");

  emit _manager->deleteFileResponse();
}

void ClientMessageProcessor::fileDeleted() {
  info("FILE::FILE_DELETED query received");

  auto fileId = _m.getInt("fileId");

  emit _manager->fileDeletedQuery(fileId);
}

void ClientMessageProcessor::getLink() {
  info("FILE::GET_LINK response received");

  auto link = _m.getString("link");

  emit _manager->getLinkResponse(link);
}

void ClientMessageProcessor::activateLink() {
  info("FILE::ACTIVATE_LINK response received");

  auto element = _m.getObject("element");
  auto file = _m.getObject("file");

  emit _manager->activateLinkResponse(FSElement::fromJsonObject(element), File::fromJsonObject(file));
}

void ClientMessageProcessor::remoteInsert() {
  info("FILE_EDIT::REMOTE_INSERT query received");

  auto symbols = _m.getArray("symbols");

  emit _manager->remoteInsertQuery(utils::jsonArrayToVector<Symbol>(symbols));
}

void ClientMessageProcessor::remoteDelete() {
  info("FILE_EDIT::REMOTE_DELETE query received");

  auto ids = _m.getArray("ids");

  emit _manager->remoteDeleteQuery(utils::jsonArrayToVector<SymbolId>(ids));
}

void ClientMessageProcessor::remoteUpdate() {
  info("FILE_EDIT::REMOTE_UPDATE query received");

  auto symbols = _m.getArray("symbols");

  emit _manager->remoteUpdateQuery(utils::jsonArrayToVector<Symbol>(symbols));
}

void ClientMessageProcessor::userConnected() {
  info("FILE_EDIT::USER_CONNECTED query received");

  auto fileId = _m.getInt("fileId");
  auto clientId = _m.getInt("clientId");
  auto username = _m.getString("username");

  emit _manager->userConnectedQuery(fileId, clientId, username);
}

void ClientMessageProcessor::userDisconnected() {
  info("FILE_EDIT::USER_DISCONNECTED query received");

  auto fileId = _m.getInt("fileId");
  auto clientId = _m.getInt("clientId");

  emit _manager->userDisconnectedQuery(fileId, clientId);
}

void ClientMessageProcessor::remoteMove() {
  info("FILE_EDIT::REMOTE_MOVE query received");

  auto fileId = _m.getInt("fileId");
  auto clientId = _m.getInt("clientId");
  auto symbolId = _m.getObject("symbolId");
  auto cursorPosition = _m.getInt("cursorPosition");

  emit _manager->remoteMoveQuery(fileId, clientId, SymbolId::fromJsonObject(symbolId), cursorPosition);
}

void ClientMessageProcessor::newDir() {
  info("FILESYSTEM::NEW_DIR response received");

  auto dirId = _m.getInt("dirId");

  emit _manager->newDirResponse(dirId);
}

void ClientMessageProcessor::editDir() {
  info("FILESYSTEM::EDIT_DIR response received");

  emit _manager->editDirResponse();
}

void ClientMessageProcessor::deleteDir() {
  info("FILESYSTEM::DELETE_DIR response received");

  emit _manager->deleteDirResponse();
}

void ClientMessageProcessor::getDir() {
  info("FILESYSTEM::GET_DIR response received");

  auto elements = _m.getArray("elements");

  emit _manager->getDirResponse(utils::jsonArrayToVector<FSElement>(elements));
}

void ClientMessageProcessor::moveFile() {
  info("FILESYSTEM::MOVE_FILE response received");

  emit _manager->moveFileResponse();
}
