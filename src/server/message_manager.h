#pragma once

#include "dependencies.h"
#include "session.h"
#include "File.h"
#include "FifoMap.h"
#include "def.h"

#include <QTcpServer>
#include <QThread>

#include <unordered_map>
#include <list>
#include <memory>

class MessageManager: public QObject {
  Q_OBJECT
public:
  MessageManager(const MessageManager&) = delete;
  MessageManager(MessageManager&&) = delete;
  MessageManager& operator=(const MessageManager&) = delete;
  MessageManager& operator=(MessageManager&&) = delete;

  static std::shared_ptr<MessageManager> get();

  ~MessageManager();

  /* gestione utenti */
  //quando il processor processa un msg di login, va aggiunto a _clients
  void addClient(quint64 clientId, std::shared_ptr<Session> session);

  /* gestione file */
  // richiesta di un file da parte di un client
  QByteArray getFile(quint64 clientId, int fileId);
  // aggiunta/rimozione/modifica di un simbolo nel file (da rivedere un attimo i parametri)
  void addSymbol(quint64 clientId, int fileId, const Symbol& sym);
  void deleteSymbol(quint64 clientId, int fileId, const SymbolId& symId);
  // per la modifica di un simbolo -> lo restituisco, e ci pensa il chiamante a modificarlo
  // in base a cosa deve modificare
  Symbol& getSymbol(quint64 clientId, int fileId, const SymbolId& symId);

  // chiusura fiel da parte del client -> rimozione da fileClients, NON dalla FifoMap
  void closeFile(quint64 clientId, int fileId);

  void sendToAll(quint64 clientId, int fileId, QByteArray data);

signals:
  void send_data(quint64 client_id, QByteArray data);
  void send_data(std::list<quint64> client_list, QByteArray data);
  void connection_error(quint64 client_id);

public slots:
  void process_data(quint64 client_id, QByteArray data);
  //void client_disconnected(quint64 client_id);
  void clientDisconnected(quint64 clientId);

private:
  static std::shared_ptr<MessageManager> instance;
  explicit MessageManager(QObject *parent = nullptr);

  //caricamento in memoria del file nella FifoMap, se non c'è già
  void loadFile(int fileId);

  bool clientIsLogged(quint64 clientId);
  bool clientHasFileOpen(quint64 clientId); // voglio sapere se ha aperto un qualunque file
  bool clientHasFileOpen(quint64 clientId, int fileId); // voglio sapere se ha aperto uno specifico file 

  typedef struct Data {
    quint64 clientId;
    int fileId;
    std::shared_ptr<Session> session;
    bool fileIsOpen;
  } Data;

  std::unordered_map<quint64, Data> _clients;                // mappa client_id -> Data
  std::unordered_map<int, std::list<quint64>> _fileClients;    // mappa file_id -> lista di client_id che lo stanno usando
  FifoMap<int, File> _openFiles{MAX_OPEN_FILES};   //un unico posto in cui sono presenti i file, con l'aggiunta di qualche handler per gestirli
};
