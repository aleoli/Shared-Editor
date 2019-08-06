# Esempio di ricezione/processamento di un messaggio

## Ipotesi

- Classe parent Message, più sottoclassi per il tipo di messaggio (MessageFile, MessageUser, ...)

- Nelle sottoclassi vi è un campo "action" che indica qual è l'azione da eseguire
- Ipotizziamo di essere nel server, nell'IN thread per un certo client
- Il json ha il formato seguente

```json
{
  "type": "FILE",
  "message": {
    "action": "CREATE",
	"name": "file1"
  }
}
```

Nell'oggetto esterno vi è un campo type, che discrimina la sottoclasse di Message, mentre vi è un campo message che contiene il messaggio vero e proprio. 



## Codice

### event loop

```cpp
while(1) {
    // leggo da socket i dati..
    data = readFromSocket(...);
        
    // costruisco oggetto json
    QJsonObject json = QJsonDocument::fromJson(data).object();
    
    // ricavo oggetto Message
    Message m = jsonParse(json);
    
    // processo il messaggio
    processMessage(m);
}
```

### jsonParse

```cpp
Message jsonParse(QJsonObject json) {
    MessageTypeEnum type = Message::convertTypeEnum(json["type"]);
    QJsonObject message = json["message"].object();
    
    // a seconda del tipo, costruisco un oggetto della sottoclasse adatta
    switch(type) {
        case M_TYPE_FILE:
            Message m = MessageFile::fromJson(message); // oppure MessageFile(message);
            return m;
            
        // altri casi..
         ...
    }
}
```

### processMessage

```cpp
void processMessage(Message &m const) {
    // a seconda del tipo, faccio cast del messaggio e chiamo metodo adatto
    switch(m.type) {
        case M_TYPE_FILE:
            processMessageFile(std::dynamic_cast<MessageFile>(m));
            break;

            // altri casi..
            ...
    }
}
```

### processMessageFile

```cpp
void processMessageFile(MessageFile &m const) {
    // a seconda dell'azione, faccio cose diverse/chiamo metodi diversi
    switch(m.action) {
        case M_ACTION_CREATE:
            // chiama callback/fai cose ...
            break;
            
            //altri casi..
            ...
    }
}
```

