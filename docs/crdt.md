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

#### Esempio
Questa era quella che avevo fatto nel laboratorio e mi sembra funzionasse abbastanza bene, divide in 3 sottocasi:
1. sto inserendo all'inizio
2. sto inserendo in mezzo
3. sto inserendo alla fine

```cpp
void SharedEditor::localInsert(int index, char value) {
    SymbolId s_id;
    s_id.client_id = this->_siteId;
    s_id.char_id = this->_counter;
    this->_counter++;
    vector<int> pos;
    if(index == 0) {
        // sta all'inizio
        if(!this->_symbols.empty()) {
            // c'è già qualcosa nel documento
            pos = this->_symbols.begin()->getPos();
        }
        pos.push_back(1);
        Symbol s{s_id, value, pos};
        this->_symbols.push_front(s);
        this->_server.send(Message{s, INSERT, this->_siteId});
    } else if(index == this->_symbols.size()) {
        // sta alla fine
        pos = (--this->_symbols.end())->getPos();
        pos[pos.size()-1]++;
        Symbol s{s_id, value, pos};
        this->_symbols.push_back(s);
        this->_server.send(Message{s, INSERT, this->_siteId});
    } else if(index > 0 && index < this->_symbols.size()) {
        // sta in mezzo
        auto it = std::next(this->_symbols.begin(), index-1);
        pos = it->getPos();
        auto pos_next = (++it)->getPos();
        if(pos.size() > pos_next.size()) {
            // sto già inserendo
            pos[pos.size()-1]++;
        } else {
            // devo inseririlo nel mezzo
            pos = pos_next;
            pos[pos.size()-1]--;
            pos.push_back(1);
        }
        Symbol s{s_id, value, pos};
        this->_symbols.insert(it, s);
        this->_server.send(Message{s, INSERT, this->_siteId});
    } else {
        // è fuori dal range
        std::stringstream ss;
        ss << std::endl << index << " è fuori dal range" << std::endl << "Numero attuale di elementi: " << this->_symbols.size() << std::endl << std::endl;
        throw IndexOutException{ss.str()};
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
