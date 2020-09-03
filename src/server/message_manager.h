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

  ~MessageManager() override;

  /* gestione utenti */
  //quando il processor processa un msg di login, va aggiunto a _clients
  void addClient(quint64 clientId, const std::shared_ptr<Session>& session, QString username);
  void removeClient(quint64 clientId);
  QString getUsername(quint64 clientId);
  std::optional<quint64> getClient(int userId);             // il client in cui è connesso l'utente
  std::list<quint64> getClientsInFile(int fileId);      // tutti i client che stanno lavorando su un file
  int getUserId(quint64 client_id);
  void fileDeleted(int file_id, int user_id);

  /* gestione file */
  // richiesta di un file da parte di un client
  const File& getFile(quint64 clientId, int fileId, std::optional<int> fileIdUser);
  // aggiunta/rimozione/modifica di un simbolo nel file (da rivedere un attimo i parametri)
  void addSymbols(quint64 clientId, int fileId, const QJsonArray& syms, const QJsonArray& pars);
  void deleteSymbols(quint64 clientId, int fileId, const QJsonArray& syms, const QJsonArray& pars);
  // per la modifica di un simbolo
  void updateSymbols(quint64 clientId, int fileId, const QJsonArray& syms, const QJsonArray& pars, const QDateTime &timestamp);

  /* gestione commenti */
  void addComment(quint64 clientId, int fileId, const QJsonObject& comment);
  void deleteComment(quint64 clientId, int fileId, const Session &session, const QJsonObject& comment);
  void updateComment(quint64 clientId, int fileId, const Session &session, const QJsonObject& comment);

  // chiusura fiel da parte del client -> rimozione da fileClients, NON dalla FifoMap
  void closeFile(quint64 clientId, int fileId, bool deleted=false);

  void sendToAll(quint64 clientId, int fileId, QByteArray data);

signals:
  void send_data(quint64 client_id, QByteArray data);
  void send_data(std::list<quint64> client_list, QByteArray data);
  void connection_error(quint64 client_id);

  void _quit();

public slots:
  void process_data(quint64 client_id, const QByteArray& data);
  void clientDisconnected(quint64 clientId);
  void quit();

private:
  static std::shared_ptr<MessageManager> instance;
  explicit MessageManager(QObject *parent = nullptr);

  //caricamento in memoria del file nella FifoMap, se non c'è già
  void loadFile(int clientId, int fileId);

  bool clientIsLogged(quint64 clientId);
  bool clientHasFileOpen(quint64 clientId); // voglio sapere se ha aperto un qualunque file
  bool clientHasFileOpen(quint64 clientId, int fileId); // voglio sapere se ha aperto uno specifico file

  typedef struct Data {
    quint64 clientId;
    int fileId;
    int fileIdUser;
    std::shared_ptr<Session> session;
    bool fileIsOpen;
    QString username;
  } Data;

  std::unordered_map<quint64, Data> _clients{};                // mappa client_id -> Data
  std::unordered_map<int, std::list<quint64>> _fileClients{};    // mappa file_id -> lista di client_id che lo stanno usando
  FifoMap<int, std::pair<QString, File>> _openFiles{MAX_OPEN_FILES};   //un unico posto in cui sono presenti i file, con l'aggiunta di qualche handler per gestirli
};
