#include "message_manager.h"

#include "exceptions.h"
#include "Message.h"
#include "server_message_processor.h"
#include "FSElement_db.h"

using namespace se_exceptions;

#include <iostream>

std::shared_ptr<MessageManager> MessageManager::instance = nullptr;

MessageManager::MessageManager(QObject *parent): QObject(parent) {
}

MessageManager::~MessageManager() {
}

std::shared_ptr<MessageManager> MessageManager::get() {
  if(instance == nullptr) {
    instance.reset(new MessageManager{});
  }
  return instance;
}

void MessageManager::process_data(int userId, QByteArray data) {
  try {
    auto msg = Message::fromQByteArray(data);
    auto mp = ServerMessageProcessor{msg, userId};

    if(!mp.hasResponse()) {
      return;
    }
    Message res = mp.getResponse();
    auto array = res.toQByteArray();

    std::cout << "OUT: " << array.data() << std::endl;
    if(mp.shouldSendToAll()) {
      this->sendToAll(userId, _clients[userId].fileId, array);
    } else {
      emit this->send_data(userId, array);
    }

  } catch(se_exceptions::SE_Exception ex) {
    std::cerr << ex.what() << std::endl;
    emit this->connection_error(userId);
    userDisconnected(userId);
  }
}

void MessageManager::addUser(std::shared_ptr<Session> session, QString username) {
  auto userId = session->getUserId();
  if(userIsLogged(userId)) {
    throw ClientLoginException{"User is already logged in"};
  }

  Data data;
  data.userId = userId;
  data.session = session;
  data.fileIsOpen = false;
  data.username = username;

  _clients[userId] = data;
}

QString MessageManager::getUsername(int userId) {
  return this->_clients[userId].username;
}

bool MessageManager::isConnected(int userId) {
  for(auto &i: this->_clients) {
    if(i.second.session->getUserId() == userId) {
      return true;
    }
  }
  return false;
}

std::list<int> MessageManager::getUsersInFile(int fileId) {
  return this->_fileClients[fileId];
}

void MessageManager::userDisconnected(int userId) {
  if(_clients.count(userId) == 0) return; //no exception here

  //se aveva un file aperto, lo rimuovo da _fileClients
  auto data = _clients[userId];
  if(data.fileIsOpen) {
    auto fileId = data.fileId;

    auto clients = this->getUsersInFile(fileId);
    for(auto &u_id: clients) {
      if(u_id == userId) {
        continue;
      }

      QJsonObject _data;
      _data["fileId"] = data.fileIdUser;
      _data["userId"] = u_id;

      auto msg = Message{Message::Type::FILE_EDIT, (int) Message::FileEditAction::USER_DISCONNECTED, Message::Status::QUERY, _data};
      this->send_data(u_id, msg.toQByteArray());
    }

    closeFile(userId, data.fileId);
  }
  if(data.session) {
    data.session->close();
  }

  _clients.erase(userId);
}

void MessageManager::sendToAll(int userId, int fileId, QByteArray data) {
  auto list = _fileClients[fileId]; //per copia
  list.remove(userId);

  if(!list.empty()) {
    emit this->send_data(list, data);
  }
}

File MessageManager::getFile(int userId, int fileId, std::optional<int> fileIdUser) {
  if(!userIsLogged(userId)) {
    throw ClientLoginException{"User is not logged in"};
  }

  if(userHasFileOpen(userId)) {
    throw ClientFileException{"User has a file opened"};
  }

  //carico file in memoria se non c'è già
  loadFile(userId, fileId);

  if(_fileClients.count(fileId) == 0) {
    _fileClients[fileId] = std::list<int>{userId};
  } else {
    _fileClients[fileId].push_back(userId);
  }

  File& f = _openFiles[fileId];
  _clients[userId].fileId = fileId;
  _clients[userId].fileIsOpen = true;
  _clients[userId].fileIdUser = fileIdUser ? *fileIdUser : fileId;

  f.addUser(userId, this->getUsername(userId));

  return f;
}

void MessageManager::loadFile(int userId, int fileId) {
  if(_openFiles.count(fileId) != 0) return;

  auto f_db = FSElement_db::get(*_clients[userId].session, fileId);

  File f = f_db.load();
  _openFiles[fileId] = f;
}

void MessageManager::addSymbol(int userId, int fileId, const Symbol& sym) {
  if(!userIsLogged(userId)) {
    throw ClientLoginException{"User is not logged in"};
  }

  if(!userHasFileOpen(userId, fileId)) {
    throw ClientFileException{"User has not opened the file"};
  }

  //sempre il check per vedere se il file è caricato in memoria
  // (potrebbe essere stato rimosso)
  loadFile(userId, fileId);

  File& f = _openFiles[fileId];
  f.remoteInsert(sym);
}

void MessageManager::deleteSymbol(int userId, int fileId, const SymbolId& symId) {
  if(!userIsLogged(userId)) {
    throw ClientLoginException{"User is not logged in"};
  }

  if(!userHasFileOpen(userId, fileId)) {
    throw ClientFileException{"User has not opened the file"};
  }

  //sempre il check per vedere se il file è caricato in memoria
  // (potrebbe essere stato rimosso)
  loadFile(userId, fileId);

  File& f = _openFiles[fileId];
  f.remoteDelete(symId);
}

void MessageManager::updateSymbol(int userId, int fileId, const Symbol& sym) {
  if(!userIsLogged(userId)) {
    throw ClientLoginException{"User is not logged in"};
  }

  if(!userHasFileOpen(userId, fileId)) {
    throw ClientFileException{"User has not opened the file"};
  }

  //sempre il check per vedere se il file è caricato in memoria
  // (potrebbe essere stato rimosso)
  loadFile(userId, fileId);

  File& f = _openFiles[fileId];
  f.remoteUpdate(sym);
}

void MessageManager::closeFile(int userId, int fileId) {
  if(!userIsLogged(userId)) {
    throw ClientLoginException{"User is not logged in"};
  }

  if(!userHasFileOpen(userId, fileId)) {
    throw ClientFileException{"User has not opened the file"};
  }

  //rimuovo da fileClients
  auto &list = _fileClients[fileId];
  list.remove(userId);
  if(list.empty()) _fileClients.erase(fileId);

  //rimuovo da _clients
  _clients[userId].fileIsOpen = false;

  _openFiles[fileId].removeUser(userId);
}

bool MessageManager::userIsLogged(int userId) {
  return _clients.count(userId) != 0;
}

bool MessageManager::userHasFileOpen(int userId, int fileId) {
  auto data = _clients[userId];

  return data.fileIsOpen && data.fileId == fileId;
}

bool MessageManager::userHasFileOpen(int userId) {
  return _clients[userId].fileIsOpen;
}
