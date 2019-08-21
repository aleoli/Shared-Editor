# ALGORITMO CRDT

## Cosa serve

- Un vettore di simboli. I simboli sono ordinati come appaiono nell'editor. Ogni simbolo contiene:

  - UID - identificativo univoco del simbolo
  - SiteId - identificativo univoco del client
  - position - vettore che indica la posizione del simbolo (struttura ad albero)
  - chr - carattere vero e proprio

  Inoltre contengono anche metadati che indicano se il simbolo è in grassetto, corsivo, font, ...

## Inserimento

### Locale

```cpp
void localInsert(int pos, char chr);
```

L'inserimento di un carattere nel testo è eseguito in una posizione assoluta `pos` .  La posizione indica la cella del vettore in cui inserire, shiftando i caratteri di uno a destra.

- Devo costruirmi l'oggetto Symbol da inserire nel vettore di simboli
  - Devo costruire il vettore `position` del simbolo, partendo dai simboli immediatamente precedente e successivo alla posizione in cui sto inserendo

#### Funzione tratta dall'algoritmo CRDT di Conclave (usata nel LAB3 malnati)
```cpp
void SharedEditor::localInsert(int index, char value) {
    std::string uid = std::to_string(this->_siteId) + ":" + std::to_string(this->_counter++);
    auto size = this->_symbols.size();

    //TODO what to do if index is >= size of vector symbols?
    if(index > size) index = size;

    //create array position
    int previous = index > 0 ? index - 1 : -1;
    int next = index == size ? -1 : index;

    std::vector<int> v1, v2;
    int siteId1, siteId2;
    siteId1 = siteId2 = this->_siteId;

    if(previous != -1) {
        auto sym = this->_symbols.at(previous);
        v1 = sym.getPosition();
        siteId1 = sym.getSiteId();
    }
    if(next != -1) {
        auto sym = this->_symbols.at(next);
        v2 = sym.getPosition();
        siteId2 = sym.getSiteId();
    }

    std::vector<int> position;

    findPosition(v1, v2, position, siteId1, siteId2);

    auto sym = Symbol(this->_siteId, value, uid, position);
    this->_symbols.emplace(this->_symbols.begin()+index, sym);

    //prepare Message
    auto m = Message(MessageType::INSERT, sym, this->_siteId);
    this->_server.send(m);
}

void SharedEditor::findPosition(std::vector<int> v1, std::vector<int> v2, std::vector<int> &position, int siteId1, int siteId2, int level) {
    int pos1, pos2;

    if(!v1.empty()) pos1 = v1.at(0);
    else pos1 = 0;

    if(!v2.empty()) pos2 = v2.at(0);
    else pos2 = static_cast<int>(pow(2, level) * 32);

    if(pos2 - pos1 > 1){
        //finished, found the position
        int pos = (pos2 + pos1) / 2; //TODO cambiare se vuoi
        position.push_back(pos);
        return;
    }

    else if(pos2 - pos1 == 1){
        //must go deeper
        position.push_back(pos1);

        if(!v1.empty()) v1.erase(v1.begin());
        v2.clear();

        findPosition(v1, v2, position, siteId1, siteId2, level + 1);
    }

    else if(pos2 == pos1) {
        //must go deeper

        if (siteId1 < siteId2) {
            position.push_back(pos1);

            if (!v1.empty()) v1.erase(v1.begin());
            v2.clear();

            findPosition(v1, v2, position, siteId1, siteId2, level + 1);
        } else if (siteId1 == siteId2) {
            position.push_back(pos1);

            if (!v1.empty()) v1.erase(v1.begin());
            if (!v2.empty()) v2.erase(v2.begin());

            findPosition(v1, v2, position, siteId1, siteId2, level + 1);
        } else {
            throw std::runtime_error("Error!!");
        }

    }
}
```

### Remoto

```cpp
void remoteInsert(Symbol sym);
```

Questa volta devo inserire direttamente l'oggetto Symbol nel vettore. Non resta che fare una scansione di esso, andando a trovare la posizione adatta.

Come funziona: (pseudocodice)

```cpp
for(int i=0; i<symbols; i++){
    Symbol s = symbols[i];
    
    if(sym.position < s.position) break; //ho trovato la posizione!
    
    if(sym.position == s.position && sym.siteId < s.siteId) break; //ho trovato la posizione!
}
symbols.insert(i, sym);
```

## Cancellazione

### Locale

```cpp
void remove(int pos);
```

Rimuovo il simbolo alla posizione pos del vettore

### Remoto

```cpp
void remove(Symbol sym);
```

Rimuovo il simbolo `sym`; comporta una ricerca del simbolo nel vettore (individuazione tramite `UID`)

## Modifica

La modifica riguarda gli attributi di stile (grassetto, corsivo, font, ...).

Il meccanismo è simile all'eliminazione: bisogna anzitutto trovare il simbolo nel vettore di simboli (esattamente come per la cancellazione), e modificare i suoi attributi. Inoltre, per il caso remoto, va anche propagata la modifica all'editor di testo.

## Strategia da utilizzare con QTextEdit

### Problemi:

- QTextEdit in sè ha solamente il segnale `textChanged` per avvisarci che qualcosa è cambiato nel testo. Non ci dice nè dove, nè quanti caratteri sono stati inseriti/rimossi. Questo è un problema: non sempre il testo è modificato nella posizione del cursore (undo/redo), quindi basarsi sul cursore non è affidabile.
- Taglia/Incolla. Come faccio a sapere quanti caratteri sto togliendo/mettendo e quali? Ci sono dei metodi che possiamo utilizzare per saperlo? Diventa complicato.
- Undo/Redo. Probabilmente le azioni più ostiche. Sono azioni che non bisogna implementare (per fortuna), ma hanno lo stesso problema del taglia/incolla, inoltre come spiegato prima possono avvenire in qualunque punto del testo.
- altro..

### Possibile soluzione

Sfruttare il segnale `contentsChange` di `QTextDocument`

```cpp
void QTextDocument::contentsChange(int position, int charsRemoved, int charsAdded);
```

- position indica la posizione in cui è avvenuta la modifica (indipendentemente dal cursore!)
- charsRemoved indica quanti caratteri sono stati rimossi
- charsAdded indica quanti caratteri sono stati aggiunti

Ogni modifica triggera l'emissione di questo segnale. Non viene detto quali caratteri sono stati rimossi/aggiunti, ma è un'informazione ricavabile facilmente.

#### Esempio d'uso

- Collegamento del signal con lo slot

```cpp
QObject::connect(textEdit->document(), &QTextDocument::contentsChange, this, &TextEdit::change);
```

- Slot

```cpp
void TextEdit::change(int pos, int removed, int added) {
	
    // verifico quali caratteri sono stati rimossi (accedendo al vettore di simboli)
    for(int i=0; i<removed; i++) {
        
        // individuo simbolo e lo rimuovo dal vettore
        auto sym = symbols.at(pos);
        symbols.erase(symbols.begin()+pos);

        // mando messaggio al server
        sendMessageDeleteSymbol(sym);
    }
	
    // verifico quali caratteri sono stati aggiunti (direttamente dall'editor)
    for(int i=0; i<added; i++) {
        
        auto chr = textEdit->document()->characterAt(pos + i);
        
		// creo simbolo..
        auto sym = createSymbol(chr, ...);
        
        //inserisco nel vettore di simboli
        symbols.insert(symbols.begin()+pos+i, sym);
        
        //mando messaggio al server
        sendMessageInsertSymbol(sym);
    }
}
```

#### Problemi / fare attenzione

- Quando effettuo una modifica di stile (grassetto, corsivo, ...) il segnale è triggerato comunque. Soluzione: negli slot collegati a queste azioni settare un flag, in modo che quando viene eseguito lo slot `TextEdit::change` non venga fatto nulla. Bisogna fare in modo che questo slot venga **sempre** eseguito **dopo** gli altri slot. (facile)
- Quando inserisco un simbolo, bisogna controllare quali attributi (sempre grassetto ecc..) sono settati, in modo da aggiungere queste informazioni all'oggetto `Symbol` che verrà creato.
