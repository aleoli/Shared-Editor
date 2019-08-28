# Compilazione con CMake

## Premessa: path delle librerie di QT

Per facilitare l'inclusione del path delle librerie di QT (quello che in CLion viene fatto nelle impostazioni, aggiungendo l'opzione `-DCMAKE_PREFIX_PATH=...`), conviene salvare in una variabile d'ambiente il path.

### comando linux (non so se funziona anche su mac)

Da terminale, inserire il seguente comando:

```bash
echo "export QT_PATH=<path>" >> ~/.bashrc
```

In cui `<path>` indica appunto il percorso in cui si trovano le librerie di QT.

**NB** Riavviare il terminale per rendere effettive le modifiche.

## Utilizzo degli script

Sono stati scritti degli script per rendere la compilazione molto semplice e veloce. Gli script si trovano nella cartella `scripts`, la quale si trova nella root del progetto.

**Utilizzare gli script da terminale, posizionandosi nella cartella root del progetto!!**

### Comando da utilizzare

```bash
# se si ha aggiunto il path di qt come variabile d'ambiente
./scripts/build_XXX.sh 

# se NON si ha aggiunto il path di qt come variabile d'ambiente
./scripts/build_XXX.sh <path librerie qt>
```

Al posto di `XXX` va messa una parola tra `client`, `server`, `all` a seconda di cosa si vuole compilare.

Esempio, volendo compilare il client:

```bash
./scripts/build_client.sh 
```

Gli script creano delle cartelle chiamate `build` nelle due sottocartelle di `src` in cui sono presenti tutti i file necessari alla compilazione, è bene non cancellarle in modo da non dover ricompilare tutto ogni volta. In ogni caso queste cartelle non verranno prese in considerazione da git per i commit.

### Risultati degli script

Gli eseguibili sono poi copiati in una cartella chiamata `bin`, che viene creata nella root del progetto.

Anche questa cartella e il suo contenuto non saranno considerate nei commit.

## Compilazione manuale

**NB** se si utilizzano gli script NON è necessario fare queste operazioni!

### compilare sia client che server

- Aprire il terminale e posizionarsi nella cartella root del progetto

- Eseguire i seguenti comandi

  ```bash
  mkdir build
  cd build
  
  # se si ha aggiunto il path di qt come variabile d'ambiente
  cmake .. -DCMAKE_PREFIX_PATH=$QT_PATH
  
  # oppure, se NON si ha aggiunto il path di qt come variabile d'ambiente
  cmake .. -DCMAKE_PREFIX_PATH=<path librerie qt>
  
  make
  ```

Automaticamente verranno creati gli eseguibili, che saranno reperibili all'interno della cartella `build`.

### compilare solo il client

- Aprire il terminale e posizionarsi in `<root_progetto>/src/client`
- Eseguire gli stessi comandi di prima

Automaticamente verranno creati gli eseguibili, che saranno reperibili all'interno della cartella `src/client/build`.

### compilare solo il server

- Aprire il terminale e posizionarsi in `<root_progetto>/src/server`
- Eseguire gli stessi comandi di prima

Automaticamente verranno creati gli eseguibili, che saranno reperibili all'interno della cartella `src/server/build`.