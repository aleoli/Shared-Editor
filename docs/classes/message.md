# Message

## Private Attributes

*  client_id: int -> che nel server può essere associato ad un utente
*  type -> enum { FILE, SIMBOLO, UTENTE }
*  status -> enum { QUERY, RESPONSE }

# MessageFile

## Private Attributes

*  action -> enum { CREAZIONE, INIZIO_MODIFICA, ... }

> in base alla action ci saranno contenuti diversi

# MessageSimbolo

## Private Attributes

*  action -> enum { INSERT, UPDATE }
*  char
*  posizione: [int]

# MessageUtente

## Private Attributes

*  action -> enum { CREAZIONE, LOGIN, ... }

> in base alla action ci saranno contenuti diversi
