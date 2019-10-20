#include "message_manager.h"

#include "exceptions.h"
#include "Message.h"
#include "server_message_processor.h"

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

  }
  catch(se_exceptions::SE_Exception ex) {
    std::cerr << ex.what() << std::endl;
    emit this->connection_error(client_id);
    clientDisconnected(client_id);
  }
}

void MessageManager::addClient(quint64 clientId, std::shared_ptr<Session> session) {
  if(clientIsLogged(clientId)) {
    throw ClientLoginException{"Client is already logged in"};
  }

  Data data;
  data.clientId = clientId;
  data.session = session;
  data.fileIsOpen = false;

  _clients[clientId] = data;
}

void MessageManager::clientDisconnected(quint64 clientId) {
  if(_clients.count(clientId) == 0) return; //no exception here

  //se aveva un file aperto, lo rimuovo da _fileClients
  auto data = _clients[clientId];
  if(data.fileIsOpen) {
    closeFile(clientId, data.fileId);
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

QByteArray MessageManager::getFile(quint64 clientId, int fileId) {
  if(!clientIsLogged(clientId)) {
    throw ClientLoginException{"Client is not logged in"};
  }

  if(clientHasFileOpen(clientId)) {
    throw ClientFileException{"Client has a file opened"};
  }

  //carico file in memoria se non c'è già
  loadFile(fileId);

  if(_fileClients.count(fileId) == 0) {
    _fileClients[fileId] = std::list<quint64>{clientId};
  }
  else {
    _fileClients[fileId].push_back(clientId);
  }

  File& f = _openFiles[fileId];
  _clients[clientId].fileId = fileId;
  _clients[clientId].fileIsOpen = true;

  return f.toQByteArray();
}

void MessageManager::loadFile(int fileId) {
  //TODO carica file da disco e mettilo nella mappa
  //TODO check se il file non esiste -> eccezione FileNotFoundException

  if(_openFiles.count(fileId) != 0) return;

  File f;
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
  loadFile(fileId);

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
  loadFile(fileId);

  File& f = _openFiles[fileId];
  f.remoteDelete(symId);
}

Symbol& MessageManager::getSymbol(quint64 clientId, int fileId, const SymbolId& symId) {
  if(!clientIsLogged(clientId)) {
    throw ClientLoginException{"Client is not logged in"};
  }

  if(!clientHasFileOpen(clientId, fileId)) {
    throw ClientFileException{"Client has not opened the file"};
  }

  //sempre il check per vedere se il file è caricato in memoria
  // (potrebbe essere stato rimosso)
  loadFile(fileId);

  File& f = _openFiles[fileId];
  return f.symbolById(symId).second;
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
