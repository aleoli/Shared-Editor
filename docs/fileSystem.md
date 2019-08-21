# Gestione del file system nel server

## Cosa ho bisogno di salvare

- Utenti: tutte le varie info di ogni utente (id, nickname, password, nome, cognome, ...)
- Files: sia elenco di caratteri (simboli) che informazioni quali nome del file e via dicendo
- Relazione Utenti - Files: Per ogni file, devo sapere chi ha il diritto di fare cosa.
- Sessioni: Quando un utente fa il login viene creato un nuovo token di sessione che viene inviato in ogni messaggio per l'autenticazione, se il token non viene usato per un certo intervallo la sessione termina

- Altro.. ?

## Utenti

Le informazioni di ogni utente possono tranquillamente essere salvate in una tabella del database. La gestione degli utenti non è un'operazione critica per la nostra applicazione.

Le icone vengono salvate come stringhe nel DB (in base64) e rimandate indietro al client che le decodifica e le visualizza
[esempio](https://stackoverflow.com/questions/27854979/how-to-load-base64-image-data-from-json-in-qt)

#### Tabella DB

Users(__nickname__, password, path_icona, nome, cognome, ...)



## Relazione Utenti - Files

Anche qui basta utilizzare una tabella nel db.

#### Tabella DB

Permissions(__ID_file__, __nickname__, is_owner, can_read, can_write, ..)



## File - Proposta

Per quanto riguarda i file, la proposta è quella di utilizzare un approccio misto:

- Una tabella nel database per le informazioni principali (id, nome, eventualmente path nel file system)
- I file veri e propri (oggetto File serializzato, contenente anche l'elenco dei simboli) salvato in un file opportuno nel file system. 

#### Tabella DB

Files(__ID_file__, name, path, ...)

### Mantenere i file in memoria

Il server non avrebbe strettamente bisogno di mantenere in memoria RAM gli oggetti File. Infatti, esso deve solamente mandare i dati ai vari client, e salvare su disco ogni operazione di modifica. Per quest'ultimo punto, potrebbe però essere utile avere il file in memoria per questioni di efficienza.

Infatti:

- Se ho il file in memoria, devo solamente applicare la modifica sull'oggetto File, e riscriverlo su disco in forma serializzata. Totale: 1 scrittura su disco
- Se non ho il file in memoria, invece, devo portarlo in memoria, fare la modifica, e scrivere su disco. Totale: 1 lettura + 1 scrittura. Questo può essere altamente inefficiente in quanto generalmente le modifiche riguardano i singoli simboli, per cui posso avere un numero molto elevato di modifiche

#### Proposta

Utilizzare un vettore di oggetti File in uso. Questo vettore ha una dimensione massima (N elementi), per non utilizzare un quantitativo di memoria troppo elevato. Inoltre il vettore è unico nel server. 

Quando devo applicare una modifica a un file, cerco nel vettore se questo è presente. Se non lo è, lo carico in memoria. Se il vettore è pieno, devo eliminare un file dalla memoria utilizzando una politica di rimpiazzamento adeguata (LRU o FIFO).

_Valutare anche se ha senso utilizzare un thread a parte per gestire le scritture su disco, in modo da velocizzare il Manager. In alternativa, utilizzare scritture su disco non bloccanti_

## Organizzazione del file system

- Tutti i file sono all'interno di una singola cartella all'interno del file system del server
  - ad esempio `/var/shared-editor/` e si chiamano ognuno con l'id del file così si è sicuri che siano univoci
- Le tabelle nel database sono:
  - File (id, owner, ...)
  - Directory (id, parent, owner, name)
  - F_in_Dir (file_id, dir_id, name)

> **NB:** il nome del file può essere messo sia in File e quindi è unico tra tutti gli utenti che l'hanno condiviso, sia in F_in_Dir e quindi ogni utente può rinominare il file in maniera differente

> Ogni utente può spostare liberamente il file nelle sue cartelle dove vuole, per lui è come se fosse suo il file

> Il file fisico è sempre lo stesso e non viene mai spostato, il che facilita la gestione in cache e in accesso multiplo

#### Richiesta del client

Tutti i file sono trattati alla stessa maniera

Alla prima richiesta mando la lista di tutte le cartelle dell'utente

> quindi con parent == NULL e owner='pippo'

Da questo punto in poi quando richiedo una cartella mi manda la lista delle cartelle e dei file contenuti all'interno, che siano suoi o di altri non cambia più niente

In sostanza è molto più intergata la gestione dei file di altri e non vi è nessuna sostanziale differenza con i propri file e visto che lo scopo e fare un editor condiviso penso che la parte più importante sia proprio quella dei file condivisi

#### Improvements

Se si vuole poter avere già dei file nella root e non dentro qualche cartella è sufficiente creare una cartella insieme all'utente che è quella di default che è anche la prima che viene aperta quando si naviga nei file e sarà questa l'unica per l'utente ad avere parent==NULL

## Invito a collaborare

> Un client può inviare ai propri conoscenti, attraverso il canale che ritiene opportuno (e-mail,
chat, …), una URI riferita al documento corrente. Copiando e incollando tale URI nel proprio
client, il destinatario dell’invito può accedere al documento e partecipare alla sua modifica

Per implementare questa feature, ci possono essere svariati modi. Analizzare le possibili alternative e scegliere quella che risulta più semplice ed efficiente.

- L'URI non deve essere "indovinabile" da nessun client

- L'invito vale una volta sola o può essere utilizzato più volte? A seconda della risposta vi possono essere strategie diverse

### Proposte

- L'invito per un certo file sarà dato da una stringa di una certa lunghezza, generata casualmente. La stringa va salvata su disco (in una tabella del db?).

  - Devo salvare informazioni aggiuntive su disco
  - Un invito può "scadere" (elimino la entry dal db), ma può anche essere sempre valido.

  

- Cifro l'invito per un certo file con un dato metodo di crittografia reversibile (utilizzando quindi una chiave, conosciuta solo dal server). Il server quindi codificherà e decodificherà gli inviti.

  - Non devo salvare informazioni su disco, oltre alla chiave di cifratura (che può essere fissa).
  - é conveniente solo se l'invito è sempre valido. Se l'invito può scadere, devo necessariamente salvare informazioni aggiuntive su disco, perdendo **di fatto** il vantaggio di utilizzare questo meccanismo
  


- altro..
