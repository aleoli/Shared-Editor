#pragma once

#include "File.h"

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
  void send_data(QByteArray data);
  void connection_error();

public slots:
  void process_data(QByteArray data);

private:
  static std::shared_ptr<MessageManager> instance;
  explicit MessageManager(QObject *parent = nullptr);

  std::shared_ptr<File> _file;
  QString _token;
};
