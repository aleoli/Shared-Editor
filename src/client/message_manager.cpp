#include "message_manager.h"

#include "exceptions.h"
#include "Message.h"
#include "client_message_processor.h"
#include "utils.h"

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

void MessageManager::process_data(const QByteArray &data) {
#if VERBOSE
  std::cout << "DATA: " << data.data() << std::endl;
#endif
  try {
    auto msg = Message::fromQByteArray(data);
    auto mp = ClientMessageProcessor{msg};
    if(!mp.hasResponse()) {
      return;
    }

    Message res = mp.getResponse();
    auto array = res.toQByteArray();
#if VERBOSE
    std::cout << "OUT: " << array.data() << std::endl;
#endif
    emit this->send_data(array);

  } catch(se_exceptions::SE_Exception ex) {
    std::cerr << ex.what() << std::endl;
    emit this->connection_error();
  }
}

// SLOT C->S

void MessageManager::loginQuery(const QString &username, const QString &password) {
  Message m{Message::Type::USER, static_cast<int>(Message::UserAction::LOGIN),
    Message::Status::QUERY};

    m.setValue("username", username);
    m.setValue("password", password);

    emit send_data(m.toQByteArray());
}

void MessageManager::logoutQuery(const QString &token) {
  Message m{Message::Type::USER, static_cast<int>(Message::UserAction::LOGOUT),
    Message::Status::QUERY};

    m.setValue("token", token);

    emit send_data(m.toQByteArray());
}

void MessageManager::newUserQuery(const QString &username, const QString &password, const QString &pswRepeat) {
  Message m{Message::Type::USER, static_cast<int>(Message::UserAction::NEW),
    Message::Status::QUERY};

    m.setValue("username", username);
    m.setValue("password", password);
    m.setValue("pswRepeat", pswRepeat);

    emit send_data(m.toQByteArray());
}

void MessageManager::editUserQuery(const QString &token, const std::optional<QString> &nickname, const std::optional<QString> &oldPassword,
  const std::optional<QString> &password, const std::optional<QString> &pswRepeat, const std::optional<QString> &icon) {
    Message m{Message::Type::USER, static_cast<int>(Message::UserAction::EDIT),
      Message::Status::QUERY};

      m.setValue("token", token);

      if(nickname)
        m.setValue("nickname", *nickname);

      if(oldPassword && password && pswRepeat) {
        m.setValue("oldPassword", *oldPassword);
        m.setValue("password", *password);
        m.setValue("pswRepeat", *pswRepeat);
      }

      if(icon)
        m.setValue("icon", *icon);

      emit send_data(m.toQByteArray());
}

void MessageManager::deleteUserQuery(const QString &token) {
  Message m{Message::Type::USER, static_cast<int>(Message::UserAction::DELETE),
    Message::Status::QUERY};

    m.setValue("token", token);

    emit send_data(m.toQByteArray());
}

void MessageManager::newFileQuery(const QString &token, const QString &name, const std::optional<int> &dirId) {
  Message m{Message::Type::FILE, static_cast<int>(Message::FileAction::NEW),
    Message::Status::QUERY};

    m.setValue("token", token);
    m.setValue("name", name);

    if(dirId)
      m.setValue("dirId", *dirId);

    emit send_data(m.toQByteArray());
}

void MessageManager::getFileQuery(const QString &token, int fileId) {
  Message m{Message::Type::FILE, static_cast<int>(Message::FileAction::GET),
    Message::Status::QUERY};

    m.setValue("token", token);
    m.setValue("fileId", fileId);

    emit send_data(m.toQByteArray());
}

void MessageManager::closeFileQuery(const QString &token, int fileId) {
  Message m{Message::Type::FILE, static_cast<int>(Message::FileAction::CLOSE),
    Message::Status::QUERY};

    m.setValue("token", token);
    m.setValue("fileId", fileId);

    emit send_data(m.toQByteArray());
}

void MessageManager::editFileQuery(const QString &token, int fileId, const std::optional<QString> &name) {
  Message m{Message::Type::FILE, static_cast<int>(Message::FileAction::EDIT),
    Message::Status::QUERY};

    m.setValue("token", token);
    m.setValue("fileId", fileId);

    if(name)
      m.setValue("name", *name);

    emit send_data(m.toQByteArray());
}

void MessageManager::deleteFileQuery(const QString &token, int fileId) {
  Message m{Message::Type::FILE, static_cast<int>(Message::FileAction::DELETE),
    Message::Status::QUERY};

    m.setValue("token", token);
    m.setValue("fileId", fileId);

    emit send_data(m.toQByteArray());
}

void MessageManager::getLinkQuery(const QString &token, int fileId) {
  Message m{Message::Type::FILE, static_cast<int>(Message::FileAction::GET_LINK),
    Message::Status::QUERY};

    m.setValue("token", token);
    m.setValue("fileId", fileId);

    emit send_data(m.toQByteArray());
}

void MessageManager::activateLinkQuery(const QString &token, const QString &link) {
  Message m{Message::Type::FILE, static_cast<int>(Message::FileAction::ACTIVATE_LINK),
    Message::Status::QUERY};

    m.setValue("token", token);
    m.setValue("link", link);

    emit send_data(m.toQByteArray());
}

void MessageManager::localInsertQuery(const QString &token, int fileId, const std::vector<Symbol> &symbols) {
  Message m{Message::Type::FILE_EDIT, static_cast<int>(Message::FileEditAction::LOCAL_INSERT),
    Message::Status::QUERY};

    m.setValue("token", token);
    m.setValue("fileId", fileId);
    m.setValue("symbols", utils::vectorToJsonArray(symbols));

    emit send_data(m.toQByteArray());
}

void MessageManager::localDeleteQuery(const QString &token, int fileId, const std::vector<SymbolId> &ids) {
  Message m{Message::Type::FILE_EDIT, static_cast<int>(Message::FileEditAction::LOCAL_DELETE),
    Message::Status::QUERY};

    m.setValue("token", token);
    m.setValue("fileId", fileId);
    m.setValue("ids", utils::vectorToJsonArray(ids));

    emit send_data(m.toQByteArray());
}

void MessageManager::localUpdateQuery(const QString &token, int fileId, const std::vector<Symbol> &symbols) {
  Message m{Message::Type::FILE_EDIT, static_cast<int>(Message::FileEditAction::LOCAL_UPDATE),
    Message::Status::QUERY};

    m.setValue("token", token);
    m.setValue("fileId", fileId);
    //TODO aggiusta per non mandare il vettore pos dei symbols
    m.setValue("symbols", utils::vectorToJsonArray(symbols));

    emit send_data(m.toQByteArray());
}

void MessageManager::localMoveQuery(const QString &token, int fileId, const SymbolId &symbolId, int cursorPosition) {
  Message m{Message::Type::FILE_EDIT, static_cast<int>(Message::FileEditAction::LOCAL_MOVE),
    Message::Status::QUERY};

    m.setValue("token", token);
    m.setValue("fileId", fileId);
    m.setValue("symbolId", symbolId.toJsonObject());
    m.setValue("cursorPosition", cursorPosition);

    emit send_data(m.toQByteArray());
}

void MessageManager::newDirQuery(const QString &token, const QString &name, const std::optional<int> &parentId) {
  Message m{Message::Type::FILESYSTEM, static_cast<int>(Message::FileSystemAction::NEW_DIR),
    Message::Status::QUERY};

    m.setValue("token", token);
    m.setValue("name", name);

    if(parentId)
      m.setValue("parentId", *parentId);

    emit send_data(m.toQByteArray());
}

void MessageManager::editDirQuery(const QString &token, int dirId, const std::optional<QString> &name, const std::optional<int> &parentId = std::nullopt) {
  Message m{Message::Type::FILESYSTEM, static_cast<int>(Message::FileSystemAction::EDIT_DIR),
    Message::Status::QUERY};

    m.setValue("token", token);
    m.setValue("dirId", dirId);

    if(name)
      m.setValue("name", *name);

    if(parentId)
      m.setValue("parentId", *parentId);

    emit send_data(m.toQByteArray());
}

void MessageManager::deleteDirQuery(const QString &token, int dirId) {
  Message m{Message::Type::FILESYSTEM, static_cast<int>(Message::FileSystemAction::DELETE_DIR),
    Message::Status::QUERY};

    m.setValue("token", token);
    m.setValue("dirId", dirId);

    emit send_data(m.toQByteArray());
}

void MessageManager::getDirQuery(const QString &token, const std::optional<int> &dirId) {
  Message m{Message::Type::FILESYSTEM, static_cast<int>(Message::FileSystemAction::GET_DIR),
    Message::Status::QUERY};

    m.setValue("token", token);
    if(dirId)
      m.setValue("dirId", *dirId);

    emit send_data(m.toQByteArray());
}

void MessageManager::moveFileQuery(const QString &token, int fileId, int dirId) {
  Message m{Message::Type::FILESYSTEM, static_cast<int>(Message::FileSystemAction::MOVE_FILE),
    Message::Status::QUERY};

    m.setValue("token", token);
    m.setValue("fileId", fileId);
    m.setValue("dirId", dirId);

    emit send_data(m.toQByteArray());
}

void MessageManager::commentLocalInsertQuery(const QString &token, int fileId, const File::Comment &comment) {
  Message m{Message::Type::COMMENT, static_cast<int>(Message::CommentAction::COMMENT_LOCAL_INSERT),
    Message::Status::QUERY};

    m.setValue("token", token);
    m.setValue("fileId", fileId);
    m.setValue("comment", File::commentToJsonObject(comment));

    emit send_data(m.toQByteArray());
}

void MessageManager::commentLocalUpdateQuery(const QString &token, int fileId, const File::Comment &comment) {
  Message m{Message::Type::COMMENT, static_cast<int>(Message::CommentAction::COMMENT_LOCAL_UPDATE),
    Message::Status::QUERY};

    m.setValue("token", token);
    m.setValue("fileId", fileId);
    m.setValue("comment", File::commentToJsonObject(comment));

    emit send_data(m.toQByteArray());
}

void MessageManager::commentLocalDeleteQuery(const QString &token, int fileId, const File::Comment &comment) {
  Message m{Message::Type::COMMENT, static_cast<int>(Message::CommentAction::COMMENT_LOCAL_DELETE),
    Message::Status::QUERY};

    m.setValue("token", token);
    m.setValue("fileId", fileId);
    m.setValue("comment", File::commentToJsonObject(comment));

    emit send_data(m.toQByteArray());
}
