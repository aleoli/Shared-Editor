# Class diagrams

## Classi in comune

![UML Class Diagram](http://www.plantuml.com/plantuml/png/ZLB1Qkim4BphAvQSd3mItbcKaXvQQA6KqDoD8Yk9YgrrjIlJOFBlbHIBv1HwCJipqkxio6jYvJbqhT2j8eBFKxV35aOGIwXj8oVYjSc4Fde9-gXyHZsI1E96oi5eHYy4ziQhM5rOOc_TePg3HaSCuYpAaD9pZ8GrZlCawrYatWNa2vEVvHBAfTBatH2fWu4nMU5JRq0MRXlHimjTKfHcY-wgFMLSLECz-YZk4Dk46yKAvArz8Vx1tRMYFX8hjleCUu_T6w6hDkdxiViockVNsV4eOzOJUNFc6iqcc91sglTsZvEHxyI9kYk-OaQcgRl0vaVKUfXeFOY3uMekOIlwmDKsLWFQvYRt5zjEUPMyOnd8-8W995oUyI4AZt_PAhQxlrAU1uluUBF_gzLYLluk2KT5OcrS4xhs5m00)

<details>
<summary> Codice PlantUML </summary>
@startuml
@startuml
class Symbol { 
- _id: SymbolId 
- _chr: char 
- _pos: std::vector

operator std::string() const 
}

class SymbolId {
client_id: int
char_id: int 
}

Symbol -- SymbolId

class Message {
- _type : MessageTypeEnum
- _action : MessageActionEnum
- _error : bool
- _data : std::string

+ operator std::string() const 
+ {static} fromJson(QJsonObject) : Message
+ toJson() : QJsonObject
}

class Queue {
_fifo: std::queue
_m: std::shared_mutex
_cv: std::cv
get(): T
put(T): void 
}

class File {
- _id: int
- _user_ids : vector<int>
+ {static} fromJson(QJsonObject) : File
+ toJson() : QJsonObject
}

File  *-- "0..*" Symbol : has
@enduml
</details>

### Spiegazione classi:

- Symbol: classe che modella un carattere. Contiene il carattere più altre informazioni quali il suo id univoco e la posizione nel file
  - fa l'override della conversione in stringa in cui restituisce il carattere contenuto
- Message: messaggio che viene scambiato tra client e server. Contiene un attributo che indica il tipo del messaggio (login, modifica, nuovo file, etc..), più altri campi che contengono i dati
  - fa l'override della conversione in stringa in cui restituisce il JSON del suo contenuto
- Queue: coda thread safe per la gestione dei messaggi. L'inserimento/prelievo di oggetti da questa coda deve essere gestito tramite attesa passiva (no busy waiting) se la coda è rispettivamente piena/vuota
  - FIFO thread safe, accesso multiplo in lettura, esclusivo in scrittura, la condition variable serve a svegliare il tutto se il lettore è in attesa che ci sia qualcosa da leggere
- File: Classe che modella un file. Contiene una serie di informazioni (nome, id..) oltre a un elenco di oggetti Symbol. Contiene inoltre l'elenco degli utenti che hanno avuto accesso a quel file in scrittura, la struttura dati contine l'id dell'utente, la posizione del cursore e lo stato attuale (viene visualizzato solo se attivo, altrimenti serve solo per rimettere l'utente nel punto di modifica in cui era quando il file viene aperto nuovamente)

## Server

![UML Class Diagram](http://www.plantuml.com/plantuml/png/SoWkIImgAStDuKhEIImkLl1Dp4jCJorIgERILiWlI2rABCalqj1Ki58eoyzCuUAgvOAgyymBoIXAJKmXgD2_j8H8bSuvcQbv9K1b2A7GujGYY813QbuAq9K0)

<details>
<summary> Codice PlantUML </summary>
@startuml
class Manager {
+ operator() : void

}

class InThread {
+ operator() : void

}

class OutThread {
+ operator() : void

}

class Client {

}

class User {

}
@enduml
</details>

### Spiegazione classi:

- Manager `SINGLETON`: Classe che fa overload dell'operatore `()`, in modo da poter essere eseguito da un thread come oggetto funzionale. Il suo ruolo è quello di processare i messaggi in entrata dai vari client, inoltre smista eventuali modifiche a tutti i client eccetto colui che lo ha generato.
- InThread: Classe che fa overload dell'operatore `()`, in modo da poter essere eseguito da un thread come oggetto funzionale. Vi è un InThread per ogni client, e il suo ruolo è quello di ricevere messaggi da esso, che poi inoltrerà al Manager
- OutThread: Classe che fa overload dell'operatore `()`, in modo da poter essere eseguito da un thread come oggetto funzionale. Vi è un OutThread per ogni client, e il suo ruolo è quello di prelevare messaggi dalla coda e spedirli al client associato.
- Client: Modella un client che si è connesso al server. Contiene gli InThread e OutThread associati, oltre all'oggetto User (se il client si è loggato), più altre informazioni.
- User: Un profilo utente. Contiene informazioni quali id, credenziali, etc..

**da vedere le classi per archiviazione, servizi di rete, etc.**

## Client

![UML Class Diagram](http://www.plantuml.com/plantuml/png/SoWkIImgAStDuKhEIImkLl3C2yaeIarCKQZcqbR8BqWjIYp9BzBGLB1IAClFJE6gvOAgyoyj8KwXSqvcQbw9Rs9UOdfg4Kx53gbvAK1700000)

<details>
<summary> Codice PlantUML </summary>
@startuml
class InThread {
+ operator() : void
}

class OutThread {
+ operator() : void
}

class ClientManager {
+ operator() : void
}
@enduml
</details>

### Spiegazione classi:

- InThread: Classe che fa overload dell'operatore `()`, in modo da poter essere eseguito da un thread come oggetto funzionale. Il suo ruolo è quello di ricevere messaggi dal server, e di processarli.
- OutThread: Classe che fa overload dell'operatore `()`, in modo da poter essere eseguito da un thread come oggetto funzionale. Il suo ruolo è quello di prelevare messaggi dalla coda e spedirli al server.
- ClientManager `SINGLETON`: si occupa allo starting di autenticarsi con il server, ricevere il SiteId, tiene traccia dei simboli e li integra tra quelli digitati dall'utente e quelli che arrivano dal server (vedi [../pyhton-demo/client/__init__.py](https://github.com/aleoli/Shared-Editor/blob/master/python-demo/client/__init__.py) per un'esempio della classe)

**da vedere le classi per interfaccia grafica, servizi di rete, etc.**
