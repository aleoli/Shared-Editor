#pragma once

#include <QObject>
#include <memory>
#include <QJsonObject>
#include <QApplication>

#include "File.h"
#include "Message.h"

class MainThread : public QObject {
  Q_OBJECT

public:
  explicit MainThread(QApplication &app);
  void start();

  File& getFile();
  // quando la TextEdit viene chiusa il file lo chiudiamo
  void closeFile();

  enum class RemoteChange {
    INSERT,
    DELETE,
    EDIT
  };

signals:
  // segnale per dire alla TextEdit che il file ha ricevuto una modifica remota
  // e perciò deve aggiornare il documento
  // per efficienza va propagata la singola modifica che è stata svolta
  // senza refreshare tutto il file intero
  void fileChanged(RemoteChange type, int position); //TODO vedere i parametri

public slots:
  // slot per la modifica del file REMOTA
  // (nel thread principale per garantire COERENZA tra oggetto File e QTextEdit)
  // deve essere collegato a un signal del messagemanager
  // questo slot sarà chiamato in uno dei seguenti casi:
  // inserimento, cancellazione o modifica di un simbolo
  // quindi lo slot dovrà fare uno switch e agire a seconda del caso
  void processRemoteMessage(const Message &m);
  // ci è stato mandato un file dal server, perciò lo salviamo
  // (e bisogna poi aprire la finestra del TextEdit)
  void setFile(const QJsonObject &file);

private:
  //tutto ciò che serve per l'interfaccia grafica
  QApplication& _app;

  std::shared_ptr<File> _file;
};
