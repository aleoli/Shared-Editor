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

void MessageManager::process_data(quint64 client_id, QByteArray data) {
  try {
    auto msg = Message::fromQByteArray(data);
    auto mp = ServerMessageProcessor{msg, client_id};

    if(!mp.hasResponse()) {
      return;
    }
    Message res = mp.getResponse();
    auto array = res.toQByteArray();

    std::cout << "OUT: " << array.data() << std::endl;
    if(mp.shouldSendToAll()) {
      this->sendToAll(client_id, _clients[client_id].fileId, array);
    } else {
      emit this->send_data(client_id, array);
    }

  } catch(se_exceptions::SE_Exception ex) {
    std::cerr << ex.what() << std::endl;
    emit this->connection_error(client_id);
    clientDisconnected(client_id);
  }
}

void MessageManager::addClient(quint64 clientId, std::shared_ptr<Session> session, QString username) {
  if(clientIsLogged(clientId)) {
    throw ClientLoginException{"Client is already logged in"};
  }
  for(auto& cl: this->_clients) {
    if(cl.second.session->getUserId() == session->getUserId()) {
      throw ClientMultipleLoginException{"User already logged with other client"};
    }
  }

  Data data;
  data.clientId = clientId;
  data.session = session;
  data.fileIsOpen = false;
  data.username = username;

  _clients[clientId] = data;
}

QString MessageManager::getUsername(quint64 clientId) {
  return this->_clients[clientId].username;
}

std::optional<quint64> MessageManager::getClient(int userId) {
  for(auto &i: this->_clients) {
    if(i.second.session->getUserId() == userId) {
      return i.second.clientId;
    }
  }
  return std::nullopt;
}

std::list<quint64> MessageManager::getClientsInFile(int fileId) {
  return this->_fileClients[fileId];
}

int MessageManager::getUserId(quint64 client_id) {
  return this->_clients[client_id].session->getUserId();
}

void MessageManager::clientDisconnected(quint64 clientId) {
  if(_clients.count(clientId) == 0) return; //no exception here

  //se aveva un file aperto, lo rimuovo da _fileClients
  auto data = _clients[clientId];
  if(data.fileIsOpen) {
    auto fileId = data.fileId;

    auto clients = this->getClientsInFile(fileId);
    for(auto &cl: clients) {
      if(cl == clientId) {
        continue;
      }
      auto userId = this->getUserId(cl);

      QJsonObject _data;
      _data["fileId"] = data.fileIdUser;
      _data["userId"] = userId;

      auto msg = Message{Message::Type::FILE_EDIT, (int) Message::FileEditAction::USER_DISCONNECTED, Message::Status::QUERY, _data};
      this->send_data(cl, msg.toQByteArray());
    }

    closeFile(clientId, data.fileId);
  }
  if(data.session) {
    data.session->close();
  }

  _clients.erase(clientId);
}

void MessageManager::sendToAll(quint64 clientId, int fileId, QByteArray data) {
  auto list = _fileClients[fileId]; //per copia
  list.remove(clientId);

  if(!list.empty()) {
    emit this->send_data(list, data);
  }
}

File MessageManager::getFile(quint64 clientId, int fileId, std::optional<int> fileIdUser) {
  if(!clientIsLogged(clientId)) {
    throw ClientLoginException{"Client is not logged in"};
  }

  if(clientHasFileOpen(clientId)) {
    throw ClientFileException{"Client has a file opened"};
  }

  //carico file in memoria se non c'è già
  loadFile(clientId, fileId);

  if(_fileClients.count(fileId) == 0) {
    _fileClients[fileId] = std::list<quint64>{clientId};
  } else {
    _fileClients[fileId].push_back(clientId);
  }

  File& f = _openFiles[fileId];
  _clients[clientId].fileId = fileId;
  _clients[clientId].fileIsOpen = true;
  _clients[clientId].fileIdUser = fileIdUser ? *fileIdUser : fileId;

  f.addUser(clientId, _clients[clientId].username);

  return f;
}

void MessageManager::loadFile(int clientId, int fileId) {
  if(_openFiles.count(fileId) != 0) return;

  auto f_db = FSElement_db::get(*_clients[clientId].session, fileId);

  File f = f_db.load();
  _openFiles[fileId] = f;
}

void MessageManager::addSymbol(quint64 clientId, int fileId, const Symbol& sym) {
  if(!clientIsLogged(clientId)) {
    throw ClientLoginException{"Client is not logged in"};
  }

  if(!clientHasFileOpen(clientId, fileId)) {
    throw ClientFileException{"Client has not opened the file"};
  }

  //sempre il check per vedere se il file è caricato in memoria
  // (potrebbe essere stato rimosso)
  loadFile(clientId, fileId);

  File& f = _openFiles[fileId];
  f.remoteInsert(sym);
}

void MessageManager::deleteSymbol(quint64 clientId, int fileId, const SymbolId& symId) {
  if(!clientIsLogged(clientId)) {
    throw ClientLoginException{"Client is not logged in"};
  }

  if(!clientHasFileOpen(clientId, fileId)) {
    throw ClientFileException{"Client has not opened the file"};
  }

  //sempre il check per vedere se il file è caricato in memoria
  // (potrebbe essere stato rimosso)
  loadFile(clientId, fileId);

  File& f = _openFiles[fileId];
  f.remoteDelete(symId);
}

void MessageManager::updateSymbol(quint64 clientId, int fileId, const Symbol& sym) {
  if(!clientIsLogged(clientId)) {
    throw ClientLoginException{"Client is not logged in"};
  }

  if(!clientHasFileOpen(clientId, fileId)) {
    throw ClientFileException{"Client has not opened the file"};
  }

  //sempre il check per vedere se il file è caricato in memoria
  // (potrebbe essere stato rimosso)
  loadFile(clientId, fileId);

  File& f = _openFiles[fileId];
  f.remoteUpdate(sym);
}

void MessageManager::closeFile(quint64 clientId, int fileId) {
  if(!clientIsLogged(clientId)) {
    throw ClientLoginException{"Client is not logged in"};
  }

  if(!clientHasFileOpen(clientId, fileId)) {
    throw ClientFileException{"Client has not opened the file"};
  }

  //rimuovo da fileClients
  auto &list = _fileClients[fileId];
  list.remove(clientId);
  if(list.empty()) _fileClients.erase(fileId);

  //rimuovo da _clients
  _clients[clientId].fileIsOpen = false;

  _openFiles[fileId].removeUser(clientId);
}

bool MessageManager::clientIsLogged(quint64 clientId) {
  return _clients.count(clientId) != 0;
}

bool MessageManager::clientHasFileOpen(quint64 clientId, int fileId) {
  auto data = _clients[clientId];

  return data.fileIsOpen && data.fileId == fileId;
}

bool MessageManager::clientHasFileOpen(quint64 clientId) {
  return _clients[clientId].fileIsOpen;
}
