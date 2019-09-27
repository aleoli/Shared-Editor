#include "message_manager.h"

#include "exceptions.h"
#include "Message.h"
#include "server_message_processor.h"

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

/*
void MessageManager::open_file(quint64 client_id, std::shared_ptr<File> file) {
  auto it = this->_clients.find(client_id);
  if(it == this->_clients.end()) {
    Data data;
    data.clientId = client_id;
    data.file = file;
    this->_clients[client_id] = data;
  } else {
    it->second.file = file;
  }
  auto it2 = this->_fileClients.find(file->getId());
  if(it2 == this->_fileClients.end()) {
    std::list<quint64> l;
    l.push_back(client_id);
    this->_fileClients[file->getId()] = l;
  } else {
    it2->second.push_back(client_id);
  }
}

void MessageManager::close_file(quint64 client_id, std::shared_ptr<File> file) {
  auto it = this->_fileClients.find(file->getId());
  if(it != this->_fileClients.end()) {
    it->second.remove_if([client_id](quint64 n) {
      return n == client_id;
    });
    if(it->second.empty()) {
      this->_fileClients.erase(it);
    }
  }
}

void MessageManager::client_disconnected(quint64 client_id) {
  this->_clients.erase(client_id);
  for(auto& it: this->_fileClients) {
    it.second.remove_if([client_id](quint64 n) {
      return n == client_id;
    });
    if(it.second.empty()) {
      this->_fileClients.erase(it.first);
    }
  }
}

void MessageManager::send_all(quint64 client_id, int file_id, QByteArray data) {
  auto it = this->_fileClients.find(file_id);
  if(it != this->_fileClients.end()) {
    std::list<quint64> l;
    for(auto& i: it->second) {
      if(client_id != i) {
        l.push_back(i);
      }
    }
    if(!l.empty()) {
      emit this->send_data(l, data);
    }
  }
}
*/

void MessageManager::addClient(quint64 clientId, std::shared_ptr<Session> session) {
  //TODO check per vedere se il client id già esiste in _clients

  Data data;
  data.clientId = clientId;
  data.session = session;
  data.fileIsOpen = false;

  _clients[clientId] = data;
}

void MessageManager::clientDisconnected(quint64 clientId) {
  if(_clients.count(clientId) == 0) return;

  //se aveva un file aperto, lo rimuovo da _fileClients
  if(_clients[clientId].fileIsOpen) {
      int fileId = _clients[clientId].fileId;
      auto &list = _fileClients[fileId];
      list.remove(clientId);
      if(list.empty()) _fileClients.erase(fileId);
  }

  _clients.erase(clientId);
}

void MessageManager::sendToAll(quint64 clientId, int fileId, QByteArray data) {
  //i check sul client e sul file sono già stati fatti in precedenza

  auto list = _fileClients[fileId]; //per copia
  list.remove(clientId);

  if(!list.empty()) {
    emit this->send_data(list, data);
  }
}

QByteArray MessageManager::getFile(quint64 clientId, int fileId) {
  //TODO se il clientId non è presente in _clients, c'è qualche problema
  // perchè nella fase di login doveva essere inserito -> lancia eccezione o non so

  //TODO se il client ha un altro file aperto nel frattempo, eccezione

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
  //TODO check se il file non esiste -> eccezione ?

  if(_openFiles.count(fileId) != 0) return;

  File f;
  _openFiles[fileId] = f;
}

void MessageManager::addSymbol(quint64 clientId, int fileId, const Symbol& sym) {
  //TODO anche qua check in _clients per vedere se il client esiste e ha quel file aperto -> se no eccezione?

  //sempre il check per vedere se il file è caricato in memoria
  // (potrebbe essere stato rimosso)
  loadFile(fileId);

  File& f = _openFiles[fileId];
  f.remoteInsert(sym);
}

void MessageManager::deleteSymbol(quint64 clientId, int fileId, const SymbolId& symId) {
  //TODO anche qua check in _clients per vedere se il client esiste e ha quel file aperto -> se no eccezione?

  //sempre il check per vedere se il file è caricato in memoria
  // (potrebbe essere stato rimosso)
  loadFile(fileId);

  File& f = _openFiles[fileId];
  f.remoteDelete(symId);
}

void MessageManager::closeFile(quint64 clientId, int fileId) {
  //TODO anche qua check in _clients per vedere se il client esiste e ha quel file aperto -> se no eccezione?

  //rimuovo da fileClients
  auto &list = _fileClients[fileId];
  list.remove(clientId);
  if(list.empty()) _fileClients.erase(fileId);

  //rimuovo da _clients
  _clients[clientId].fileIsOpen = false;
}
