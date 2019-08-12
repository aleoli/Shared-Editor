# Gestione del file system nel server

## Cosa ho bisogno di salvare

- Utenti: tutte le varie info di ogni utente (id, nickname, password, nome, cognome, ...)
- Files: sia elenco di caratteri (simboli) che informazioni quali nome del file e via dicendo
- Relazione Utenti - Files: Per ogni file, devo sapere chi ha il diritto di fare cosa.
- Sessioni: Quando un utente fa il login viene creato un nuovo token di sessione che viene inviato in ogni messaggio per l'autenticazione, se il token non viene usato per un certo intervallo la sessione termina

- Altro.. ?

## Utenti

Le informazioni di ogni utente possono tranquillamente essere salvate in una tabella del database. La gestione degli utenti non è un'operazione critica per la nostra applicazione.

Le icone possono essere salvate tutte insieme in una determinata cartella del file system, oppure dentro la cartella dedicata all'utente (in cui ci sono anche i files)

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

### Organizzazione del file system

Il file system può essere organizzato come segue:

- Una cartella "root" principale
- X sottocartelle principali per i vari utenti
- Per ogni utente, un elenco di file e cartelle a libera composizione dell'utente

#### Esempio

`<main_path>/root/gianlu33/documenti vari/file_system.sed`

L'utente `gianlu33` ha creato una subdirectory chiamata `documenti vari`, in cui ha creato un file di nome `file_system`

#### QT - client, visualizzare files e cartelle di un utente

Vedere bene le librerie QT perchè probabilmente è presente qualche classe per la visualizzazione dei files e cartelle da file system in maniera molto semplice.

Guardare anche il laboratorio 5 di Malnati.

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
