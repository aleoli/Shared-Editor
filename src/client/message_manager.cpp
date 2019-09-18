#include "message_manager.h"

#include "def.h"
#include "exceptions.h"
#include "Message.h"
#include "client_message_processor.h"

#include <iostream>

#include <QJsonDocument>

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

void MessageManager::process_data(QByteArray data) {
  std::cout << "DATA: " << data.data() << std::endl;
  try {
#if SAVE_BINARY
    auto doc = QJsonDocument::fromBinaryData(data);
#else
    auto doc = QJsonDocument::fromJson(data);
#endif
    Message msg{doc.object()};
    auto mp = ClientMessageProcessor{msg};
    if(!mp.hasResp()) {
      return;
    }
    Message res = mp;
    QJsonDocument res_doc(res.toJsonObject());
#if SAVE_BINARY
    auto array = res_doc.toBinaryData();
#else
    auto array = res_doc.toJson(QJsonDocument::Compact);
#endif
    std::cout << "OUT: " << array.data() << std::endl;
    emit this->send_data(array);
  } catch(se_exceptions::SE_Exception ex) {
    std::cerr << ex.what() << std::endl;
    emit this->connection_error();
    return;
  }
}
