#include "message_manager.h"

#include "exceptions.h"
#include "Message.h"
#include "client_message_processor.h"

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

void MessageManager::process_data(QByteArray data) {
  std::cout << "DATA: " << data.data() << std::endl;
  try {
    auto msg = Message::fromQByteArray(data);
    auto mp = ClientMessageProcessor{msg};
    if(!mp.hasResponse()) {
      return;
    }

    Message res = mp.getResponse();
    auto array = res.toQByteArray();
    std::cout << "OUT: " << array.data() << std::endl;
    emit this->send_data(array);

  } catch(se_exceptions::SE_Exception ex) {
    std::cerr << ex.what() << std::endl;
    emit this->connection_error();
  }
}
