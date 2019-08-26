# Esempio di ricezione/processamento di un messaggio

## Formato JSON

- Il json ha il formato seguente

```json
{
  "type": "FILE",
  "action": "CREATE",
  "error" : false,
  "data": {
	"name": "file1"
  }
}
```

I campi `type` e `action` servono a individuare il corretto tipo del messaggio, mentre il campo `data` è un oggetto json che contiene tutti i campi utili per quel messaggio. Ogni messaggio avrà chiaramente campi diversi.

## Codice

### event loop

```cpp
while(1) {
    // leggo da socket i dati..
    data = readFromSocket(...);
        
    // costruisco oggetto json
    QJsonObject json = QJsonDocument::fromJson(data).object();
    
    // ricavo oggetto Message
    Message m = Message::fromJson(json);
    
    // processo il messaggio
    processMessage(m);
}
```

### Message::fromJson

```cpp
Message Message::fromJson(QJsonObject json) {
    MessageTypeEnum type = Message::convertTypeEnum(json["type"]);
    MessageActionEnum action = Message::convertActionEnum(json["action"]);
    bool error = json["error"].toBool();
    QJsonObject data = json["data"].toObject();
    
    return Message(type, action, error, data);
}
```

### processMessage

```cpp
void processMessage(Message &m const) {
    // a seconda del tipo, chiamo metodo adatto in cui verrà verificata l'azione
    switch(m.type) {
        case M_TYPE_FILE:
            processMessageFile(m);
            break;

            // altri casi..
            ...
    }
}
```

### processMessageFile

```cpp
void processMessageFile(Message &m const) {
    // a seconda dell'azione, chiamo callback adatta
    switch(m.action) {
        case M_ACTION_CREATE:
            createFile(m);
            break;
            
            //altri casi..
            ...
    }
}
```

### createFile

```cpp
void createFile(Message &m const) {
    QJsonObject data = m.getData();
    auto name = data["name"].toString();
    
    // crea file etc...
}
```

