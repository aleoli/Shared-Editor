#pragma once

#include "dependencies.h"
#include "session.h"
#include "File.h"

#include <QTcpServer>
#include <QThread>

#include <map>
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

signals:
  void send_data(quint64 client_id, QByteArray data);
  void send_data(std::list<quint64> client_list, QByteArray data);

public slots:
  void process_data(quint64 client_id, QByteArray data);
  void client_disconnected(quint64 client_id);

private:
  typedef struct Data {
    quint64 client_id;
    std::shared_ptr<Session> session;
    std::shared_ptr<File> file;
  } Data;

  static std::shared_ptr<MessageManager> instance;
  explicit MessageManager(QObject *parent = nullptr);

  void open_file(quint64 client_id, std::shared_ptr<File> file);
  void close_file(quint64 client_id, std::shared_ptr<File> file);

  void send_all(quint64 client_id, int file_id, QByteArray data);

  std::map<quint64, Data> _data_env;                // mappa client_id -> Data
  std::map<int, std::list<quint64>> _open_files;    // mappa file_id -> lista di client_id che lo stanno usando
};
