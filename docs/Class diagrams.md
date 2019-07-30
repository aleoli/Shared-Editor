# Class diagrams

## Classi in comune

![UML Class Diagram](http://www.plantuml.com/plantuml/png/SoWkIImgAStDuU9ApaaiBbO8hinDoizHgERYgeK9-QOM5oUcfwA81PQcbe95449kcJbm4OVKl1IWeG00)

<details>
<summary> Codice PlantUML </summary>
@startuml
class Symbol {

}

class Message {

}

class Queue {


}

class File {


}
@enduml
</details>

### Spiegazione classi:

- Symbol: classe che modella un carattere. Contiene il carattere più altre informazioni quali il suo id univoco e la posizione nel file
- Message: messaggio che viene scambiato tra client e server. Contiene un attributo che indica il tipo del messaggio (login, modifica, nuovo file, etc..), più altri campi che contengono i dati
- Queue: coda thread safe per la gestione dei messaggi. L'inserimento/prelievo di oggetti da questa coda deve essere gestito tramite attesa passiva (no busy waiting) se la coda è rispettivamente piena/vuota
- File: Classe che modella un file. Contiene una serie di informazioni (nome, id..) oltre a un elenco di oggetti Symbol

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

- Manager: Classe che fa overload dell'operatore `()`, in modo da poter essere eseguito da un thread come oggetto funzionale. Il suo ruolo è quello di processare i messaggi in entrata dai vari client, inoltre smista eventuali modifiche a tutti i client eccetto colui che lo ha generato.
- InThread: Classe che fa overload dell'operatore `()`, in modo da poter essere eseguito da un thread come oggetto funzionale. Vi è un InThread per ogni client, e il suo ruolo è quello di ricevere messaggi da esso, che poi inoltrerà al Manager
- OutThread: Classe che fa overload dell'operatore `()`, in modo da poter essere eseguito da un thread come oggetto funzionale. Vi è un OutThread per ogni client, e il suo ruolo è quello di prelevare messaggi dalla coda e spedirli al client associato.
- Client: Modella un client che si è connesso al server. Contiene gli InThread e OutThread associati, oltre all'oggetto User (se il client si è loggato), più altre informazioni.
- User: Un profilo utente. Contiene informazioni quali id, credenziali, etc..

**da vedere le classi per archiviazione, servizi di rete, etc.**

## Client

![UML Class Diagram](http://www.plantuml.com/plantuml/png/SoWkIImgAStDuKhEIImkLl3C2yaeIarCKQZcqbR8BqWjIYp9BzBGLB1IAClFJE7YgkM2AlGlBI6aqY4rBmLeCW00)

<details>
<summary> Codice PlantUML </summary>
@startuml
class InThread {
+ operator() : void

}

class OutThread {
+ operator() : void

}
@enduml
</details>

### Spiegazione classi:

- InThread: Classe che fa overload dell'operatore `()`, in modo da poter essere eseguito da un thread come oggetto funzionale. Il suo ruolo è quello di ricevere messaggi dal server, e di processarli.
- OutThread: Classe che fa overload dell'operatore `()`, in modo da poter essere eseguito da un thread come oggetto funzionale. Il suo ruolo è quello di prelevare messaggi dalla coda e spedirli al server.

**da vedere le classi per interfaccia grafica, servizi di rete, etc.**
