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
#include <optional>

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
  void addUser(std::shared_ptr<Session> session, QString username);
  QString getUsername(int userId);
  bool isConnected(int userId);             // il client in cui è connesso l'utente
  std::list<int> getUsersInFile(int fileId);      // tutti gli user che stanno lavorando su un file

  /* gestione file */
  // richiesta di un file da parte di un client
  File getFile(int userId, int fileId, std::optional<int> fileIdUser);
  // aggiunta/rimozione/modifica di un simbolo nel file (da rivedere un attimo i parametri)
  void addSymbol(int userId, int fileId, const Symbol& sym);
  void deleteSymbol(int userId, int fileId, const SymbolId& symId);
  // per la modifica di un simbolo
  void updateSymbol(int userId, int fileId, const Symbol& sym);

  // chiusura fiel da parte del client -> rimozione da fileClients, NON dalla FifoMap
  void closeFile(int userId, int fileId);

  void sendToAll(int userId, int fileId, QByteArray data);

signals:
  void send_data(int userId, QByteArray data);
  void send_data(std::list<int> user_list, QByteArray data);
  void connection_error(int userId);

public slots:
  void process_data(int userId, QByteArray data);
  void userDisconnected(int userId);

private:
  static std::shared_ptr<MessageManager> instance;
  explicit MessageManager(QObject *parent = nullptr);

  //caricamento in memoria del file nella FifoMap, se non c'è già
  void loadFile(int userId, int fileId);

  bool userIsLogged(int userId);
  bool userHasFileOpen(int userId); // voglio sapere se ha aperto un qualunque file
  bool userHasFileOpen(int userId, int fileId); // voglio sapere se ha aperto uno specifico file

  typedef struct Data {
    int userId;
    int fileId;
    int fileIdUser;
    std::shared_ptr<Session> session;
    bool fileIsOpen;
    QString username;
  } Data;

  std::unordered_map<int, Data> _clients;                // mappa client_id -> Data
  std::unordered_map<int, std::list<int>> _fileClients;    // mappa file_id -> lista di client_id che lo stanno usando
  FifoMap<int, File> _openFiles{MAX_OPEN_FILES};   //un unico posto in cui sono presenti i file, con l'aggiunta di qualche handler per gestirli
};
