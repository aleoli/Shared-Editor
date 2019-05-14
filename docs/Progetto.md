# Moduli
*  Server
*  Client

## Server
### Threads
*  Listener -> a ogni nuova connessione istanzia il socket e un nuovo thread detached per la gestione di quel client
> **NB** bisogna tenere una lista dei client attivi, su quale documento stanno lavorando e su quale socket bisogna parlargli
*  Gestore Messaggi -> si deve occupare di inviare i messaggi ricevuti da un client agli altri client attivi su quel documento

#### Proposta
Il socket creato viene assegnato ad un thread che a sua volta si sgancia 2 thread, uno in `entrata` e l'altro in `uscita` che rispettivamente leggono e scrivono sempre sullo stesso socket
*  si deve settare un timer di timeout che deve essere azzerato ad ogni comunicazione ricevuta -> se scatta il timeout si manda una specie di ping al client che se non risponde allora non è più connesso

Il thread in uscita deve avere una coda di messaggi in uscita che viene riempita dal gestore di messaggi

Il thread in entrata invece inserisce il nuovo messaggio ricevuto nella coda del gestore di messaggi perchè venga processato

### Archiviazione
> **Domanda:** dobbiamo implementarci qualcosa o possiamo usare un database?

Ogni volta che un client ci notifica una modifica del file il file deve essere salvato automaticamente

L'archiviazione deve mantenere:
1.  elenco file (id, nome, timestamp, ...)
2.  eleco utenti (id, profilo, ...)
3.  relazioni file-utenti (chi ha il diritto di modificare cosa)
4.  files (cartella)

> per ogni file l'elenco dei simboli al suo interno -> non la stringa del file ma proprio gli oggetti `simbolo`

## Client
All'**apertura** deve collegarsi con il server
1.  richiede username e password
2.  invia al server che risponde con il profilo utente
3.  richiesta dei file presenti sul server
4.  mostra una schermata in cui scegliere se modificare uno dei file o crearne uno nuovo

> si potrebbe tenere un file di configurazione in cui si salvano username e password

Durante la **modifica** di un file
1.  inserisco/rimuovo carettere dalla lista locale
2.  aggiungo messaggio alla coda dei messaggi in uscita

-> ci deve essere un thread che si occupa di mandare i messaggi da una coda

-> un altro thread sta in ascolto dal server per messaggi in entrata

## API
```
NCS: notifica del client al server
RCS: richiesta del client al server
NSC: notifica del server al client
```
Operazioni che devono essere permesse

### File
*  creazione (NCS)
*  inizio modifica (NCS) -> ottengo la lista dei simboli all'interno
*  fine modifica (NCS)
*  elimina (NCS)
*  lista file disponibili all'utente (RCS)
*  lista utenti attualmente in modifica (RCS)
*  nuovo utente in modifica (NSC)
*  rimuovi utente in modifica (NSC)

### Simbolo (dato il file)
*  insert (NCS, NSC)
*  remove (NCS, NSC)

### Utente
*  creazione (NCS)
*  login (RCS)
*  modifica (RCS)
*  logout (NCS)
*  eliminazione (non fondamentale) (RCS)
