#include "client_message_processor.h"

#include "utils.h"
#include "exceptions.h"

#include <QDateTime>

using namespace se_exceptions;

ClientMessageProcessor::ClientMessageProcessor(const Message &m): MessageProcessor(m) {
  _manager = MessageManager::get();
  this->process_message();
}

void ClientMessageProcessor::process_error() {
  info("ERROR message received");
  try {
    auto reason = _m.getString("reason");

    debug(reason);

    emit _manager->errorResponse(reason);
  }
  catch(SE_Exception& e) {
    disconnect(e.what());
  }
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

    case Message::UserAction::GET_USER_ICON:
      if(!isResponse)
        disconnect("Ricevuto messaggio con status non valido");
      else
        getUserIcon();
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

    case Message::FileAction::FILE_INFO:
      if(!isResponse)
        disconnect("Ricevuto messaggio con status non valido");
      else
        getFileInfo();
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

void ClientMessageProcessor::process_comment() {
  auto action = static_cast<Message::CommentAction>(_m.getAction());
  bool isResponse = _m.getStatus() == Message::Status::RESPONSE;

  switch(action) {
    case Message::CommentAction::COMMENT_REMOTE_INSERT:
      if(isResponse)
        disconnect("Ricevuto messaggio con status non valido");
      else
        newComment();
      break;

    case Message::CommentAction::COMMENT_REMOTE_UPDATE:
      if(isResponse)
        disconnect("Ricevuto messaggio con status non valido");
      else
        updateComment();
      break;

    case Message::CommentAction::COMMENT_REMOTE_DELETE:
      if(isResponse)
        disconnect("Ricevuto messaggio con status non valido");
      else
        deleteComment();
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

    case Message::FileSystemAction::GET_PATH:
      if(!isResponse)
        disconnect("Ricevuto messaggio con status non valido");
      else
        getPath();
      break;

    case Message::FileSystemAction::GET_ALL_DIRS:
      if(!isResponse)
        disconnect("Ricevuto messaggio con status non valido");
      else
        getAllDirs();
      break;

    case Message::FileSystemAction::SEARCH:
      if(!isResponse)
        disconnect("Ricevuto messaggio con status non valido");
      else
        search();
      break;

    default:
      disconnect("Ricevuto messaggio con azione non valida");
  }
}

void ClientMessageProcessor::disconnect(const QString& why) {
  error(why);
  emit _manager->connection_error();
}

void ClientMessageProcessor::login() {
  info("Login response received");

  try {
    auto token = _m.getString("token");
    auto userId = _m.getInt("userId");
    auto icon = _m.getStringOpt("icon");

    emit _manager->loginResponse(token, userId, icon);
  }
  catch(SE_Exception& e) {
    disconnect(e.what());
  }
}

void ClientMessageProcessor::newUser() {
  info("USER::NEW response received");

  try {
    auto token = _m.getString("token");
    auto userId = _m.getInt("userId");

    emit _manager->newUserResponse(token, userId);
  }
  catch(SE_Exception& e) {
    disconnect(e.what());
  }
}

void ClientMessageProcessor::editUser() {
  info("USER::EDIT response received");

  emit _manager->editUserResponse();
}

void ClientMessageProcessor::deleteUser() {
  info("USER::DELETE response received");

  emit _manager->deleteUserResponse();
}

void ClientMessageProcessor::getUserIcon() {
  info("USER::GET_USER_ICON response received");

  try {
    auto userId = _m.getInt("userId");
    auto found = _m.getBool("found");
    auto icon = _m.getStringOpt("icon");

    emit _manager->getIconResponse(userId, found, icon);
  }
  catch(SE_Exception& e) {
    disconnect(e.what());
  }
}

void ClientMessageProcessor::newFile() {
  info("FILE::NEW response received");

  try {
    auto fileId = _m.getInt("fileId");

    emit _manager->newFileResponse(fileId);
  }
  catch(SE_Exception& e) {
    disconnect(e.what());
  }
}

void ClientMessageProcessor::getFile() {
  info("FILE::GET response received");

  try {
    auto file = _m.getObject("file");
    auto charId = _m.getInt("charId");
    auto commentId = _m.getInt("commentId");

    emit _manager->getFileResponse(File::fromJsonObject(file), charId, commentId);
  }
  catch(SE_Exception& e) {
    disconnect(e.what());
  }
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

  try {
    auto fileId = _m.getInt("fileId");

    emit _manager->fileDeletedQuery(fileId);
  }
  catch(SE_Exception& e) {
    disconnect(e.what());
  }
}

void ClientMessageProcessor::getLink() {
  info("FILE::GET_LINK response received");

  try {
    auto link = _m.getString("link");

    emit _manager->getLinkResponse(link);
  }
  catch(SE_Exception& e) {
    disconnect(e.what());
  }
}

void ClientMessageProcessor::activateLink() {
  info("FILE::ACTIVATE_LINK response received");

  try {
    auto element = _m.getObject("element");
    auto file = _m.getObject("file");

    emit _manager->activateLinkResponse(FSElement::fromJsonObject(element), File::fromJsonObject(file));
  }
  catch(SE_Exception& e) {
    disconnect(e.what());
  }
}

void ClientMessageProcessor::getFileInfo() {
  info("FILE::FILE_INFO response received");

  try {
    FSElement::FileInfo fi;
    fi.name = _m.getString("name");
    fi.path = _m.getString("path");
    fi.owner = _m.getString("owner");
    fi.created = QDateTime::fromString(_m.getString("created"));

    emit _manager->fileInfoResponse(fi);
  }
  catch(SE_Exception& e) {
    disconnect(e.what());
  }
}

void ClientMessageProcessor::remoteInsert() {
  info("FILE_EDIT::REMOTE_INSERT query received");

  try {
    auto fileId = _m.getInt("fileId");
    auto userId = _m.getInt("userId");
    auto symbols = _m.getArray("symbols");
    auto paragraphs = _m.getArray("paragraphs");

    emit _manager->remoteInsertQuery(fileId, userId, Symbol::jsonArrayToSymbols(symbols), Paragraph::jsonArrayToParagraphs(paragraphs));
  }
  catch(SE_Exception& e) {
    disconnect(e.what());
  }
}

void ClientMessageProcessor::remoteDelete() {
  info("FILE_EDIT::REMOTE_DELETE query received");

  try {
    auto fileId = _m.getInt("fileId");
    auto userId = _m.getInt("userId");
    auto ids = _m.getArray("ids");
    auto paragraphs = _m.getArray("paragraphs");

    emit _manager->remoteDeleteQuery(fileId, userId, utils::jsonArrayToList<SymbolId>(ids), utils::jsonArrayToList<ParagraphId>(paragraphs));
  }
  catch(SE_Exception& e) {
    disconnect(e.what());
  }
}

void ClientMessageProcessor::remoteUpdate() {
  info("FILE_EDIT::REMOTE_UPDATE query received");

  try {
    auto fileId = _m.getInt("fileId");
    auto userId = _m.getInt("userId");
    auto symbols = _m.getArray("symbols");
    auto paragraphs = _m.getArray("paragraphs");
    auto timestamp = _m.getString("timestamp");

    emit _manager->remoteUpdateQuery(fileId, userId, Symbol::jsonArrayToSymbols(symbols), Paragraph::jsonArrayToParagraphs(paragraphs), QDateTime::fromString(timestamp, "dd.MM.yyyy hh:mm:ss.zzz t"));
  }
  catch(SE_Exception& e) {
    disconnect(e.what());
  }
}

void ClientMessageProcessor::userConnected() {
  info("FILE_EDIT::USER_CONNECTED query received");

  try {
    auto fileId = _m.getInt("fileId");
    auto userId = _m.getInt("userId");
    auto username = _m.getString("username");

    emit _manager->userConnectedQuery(fileId, userId, username);
  }
  catch(SE_Exception& e) {
    disconnect(e.what());
  }
}

void ClientMessageProcessor::userDisconnected() {
  info("FILE_EDIT::USER_DISCONNECTED query received");

  try {
    auto fileId = _m.getInt("fileId");
    auto userId = _m.getInt("userId");

    emit _manager->userDisconnectedQuery(fileId, userId);
  }
  catch(SE_Exception& e) {
    disconnect(e.what());
  }
}

void ClientMessageProcessor::remoteMove() {
  info("FILE_EDIT::REMOTE_MOVE query received");

  try {
    auto fileId = _m.getInt("fileId");
    auto userId = _m.getInt("userId");
    auto symbolId = _m.getObject("symbolId");
    auto cursorPosition = _m.getInt("cursorPosition");

    emit _manager->remoteMoveQuery(fileId, userId, SymbolId::fromJsonObject(symbolId), cursorPosition);
  }
  catch(SE_Exception& e) {
    disconnect(e.what());
  }
}

void ClientMessageProcessor::newDir() {
  info("FILESYSTEM::NEW_DIR response received");

  try {
    auto dirId = _m.getInt("dirId");

    emit _manager->newDirResponse(dirId);
  }
  catch(SE_Exception& e) {
    disconnect(e.what());
  }
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

  try {
    auto elements = _m.getArray("elements");
    auto name = _m.getString("name");
    auto parentId = _m.getInt("parent");

    emit _manager->getDirResponse(utils::jsonArrayToVector<FSElement>(elements), name, parentId);
  }
  catch(SE_Exception& e) {
    disconnect(e.what());
  }
}

void ClientMessageProcessor::moveFile() {
  info("FILESYSTEM::MOVE_FILE response received");

  emit _manager->moveFileResponse();
}

void ClientMessageProcessor::getPath() {
  info("FILESYSTEM::GET_PATH response received");

  try {
    auto elements = _m.getArray("elements");

    emit _manager->getPathResponse(utils::jsonArrayToVector<FSElement>(elements));
  }
  catch(SE_Exception& e) {
    disconnect(e.what());
  }
}

void ClientMessageProcessor::getAllDirs() {
  info("FILESYSTEM::GET_ALL_DIRS response received");

  try {
    auto elements = _m.getArray("elements");

    std::list<std::pair<QString, int>> l;
    for(const auto& item: elements) {
      auto obj = item.toObject();
      auto path = obj["path"].toString();
      auto id = obj["id"].toInt();
      l.emplace_back(std::pair<QString, int>{path, id});
    }

    emit _manager->getAllDirsResponse(l);
  }
  catch(SE_Exception& e) {
    disconnect(e.what());
  }
}

void ClientMessageProcessor::search() {
  info("FILESYSTEM::SEARCH response received");

  try {
    auto elements = _m.getArray("elements");

    std::list<SearchResult> res;
    for(const auto& el: elements) {
      auto obj = el.toObject();
      SearchResult sr;
      sr.id = obj["id"].toInt();
      sr.path = obj["path"].toString();
      sr.isDir = obj["isDir"].toBool();
      res.push_back(sr);
    }

    emit _manager->searchResponse(res);
  }
  catch(SE_Exception& e) {
    disconnect(e.what());
  }
}

void ClientMessageProcessor::newComment() {
  info("COMMENT::REMOTE_INSERT query received");

  try {
    auto fileId = _m.getInt("fileId");
    auto userId = _m.getInt("userId");
    auto comment = _m.getObject("comment");

    emit _manager->commentRemoteInsertQuery(fileId, userId, File::commentFromJsonObject(comment));
  }
  catch(SE_Exception& e) {
    disconnect(e.what());
  }
}

void ClientMessageProcessor::updateComment() {
  info("COMMENT::REMOTE_UPDATE query received");

  try {
    auto fileId = _m.getInt("fileId");
    auto userId = _m.getInt("userId");
    auto comment = _m.getObject("comment");

    emit _manager->commentRemoteUpdateQuery(fileId, userId, File::commentFromJsonObject(comment));
  }
  catch(SE_Exception& e) {
    disconnect(e.what());
  }
}

void ClientMessageProcessor::deleteComment() {
  info("COMMENT::REMOTE_DELETE query received");

  try {
    auto fileId = _m.getInt("fileId");
    auto userId = _m.getInt("userId");
    auto comment = _m.getObject("comment");

    emit _manager->commentRemoteDeleteQuery(fileId, userId, File::commentFromJsonObject(comment));
  }
  catch(SE_Exception& e) {
    disconnect(e.what());
  }
}
