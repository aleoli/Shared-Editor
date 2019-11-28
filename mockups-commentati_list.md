# Finestra di Login

![](mockups-commentati/Qt%20Login-commentato.png)

# Form di registrazione

![](mockups-commentati/Qt%20FormRegister-commentato.png)

# Pannello di modifica utente

![](mockups-commentati/Qt%20EditAccount-commentato.png)

### Klajdi
>1. Implementata possibilità di cambiare foto utente.

# Browser dei documenti

![](mockups-commentati/Qt%20DocsBrowser-commentato.png)

### Klajdi
>1. Come mai devo togliere l'opzione "Scarica documento"? E' fattibile implementarla.
>2. Una possibile implementazione alternativa per il pulsante "+"?
>3. Voce "Ultimo accesso" cambiata con "Data creazione".

### Bob

> 1. Perchè non credo si possa fare qui in quanto la possibilità di convertire in PDF il documento è "offerta" dalla classe QTextEdit quindi dall'editor di testo. Nel browser documenti non c'è modo di fare questa cosa a meno di magheggi strani. In conclusione quindi eliminerei da qua questa opzione, lasciandola solo nell'editor di testo
> 2. Pensavo ad avere una toolbar in alto come nell'editor di testo con i pulsanti per le varie azioni (crea file, crea cartella, torna indietro, etc...). In questo modo il "centralWidget" può essere anche più facile da gestire. Comunque questa è una vostra scelta, la mia è solo un'idea :)
> 3. Ci stava anche Ultimo accesso, dipende cosa volete fare, ho sottolineato in giallo perchè è una cosa che implementeremo in futuro

# Editor di testo

![](mockups-commentati/Qt%20textEditor-commentato-02.png)

### Klajdi

>1. (Blu): tasti undo e redo.
>2. (Verde): Possibile alternativa per il pulsante "Condividi"?

### Bob

> 1. Ok perfetto allora
> 2. Pensavo che ci stesse meglio un'icona invece di una scritta testuale, visto che per gli altri due pulsanti in quella zona ci sono icone. Anche qua è una vostra scelta

# Tendine nell'editor di testo

![](mockups-commentati/TendineTextEditor-01.png)

### Klajdi
>1. (Blu): "Elimina" = elimina il documento in questione dalla lista documenti.
>2. (Blu):"Linea orizzontale" = linea di header che separa un testo da quello sotto.
>3. (Blu):"Importa" = permette di importare un documento da locale, sostituendolo con quello attuale.
>4. (Blu):"Dettagli" = Dettagli documento: data creazione, nome documento, con chi è condiviso, ecc...
>5. (Blu):"Cancella formattazione" = elimina lo stile grafico usato nel documento (font, grassetto, corsivo, ecc...).

### Bob

> 1. E no, questa azione è gestita dall'altro pulsante "Elimina" della tendina "File" no? quello li che ti ho cerchiato non capisco ancora a cosa serva
> 2. Ok questo si può eliminare allora perchè non la gestiamo, e non so neanche se la gestisce il QTextEdit
> 3. Questo la vedo molto difficile non per il testo in sè ma per la formattazione. Che tipo di file si può importare da locale? Pdf? Altro? Non saprei come gestire i caratteri e se sono in grassetto, font, etc.. 
>    Inoltre, essendo un documento condiviso, se uno degli utenti facesse una cosa del genere agli altri penso girerebbero un pò le scatole ahah
> 4. Ok va bene, alcuni dettagli saranno "gialli" ma altri si possono già avere nella versione 1.0
> 5. Ok va bene

<!--# Vista relazione finestre

<!--![](mockups-commentati/Overview.png)

<!--# Vista completa relazione finestre

<!--![](mockups-commentati/OverviewFull.png)
