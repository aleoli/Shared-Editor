# Client Server Communication

## Legenda

* Verde -> messaggio autenticato
  * deve includere il token di sessione
  * può ottenere come risposta un 'accesso non consentito'



## new_user

![](https://github.com/aleoli/Shared-Editor/blob/master/docs/images/cs_comm/new_user.png?raw=true)



## login

![](https://github.com/aleoli/Shared-Editor/blob/master/docs/images/cs_comm/login.png?raw=true)



## new_dir

![](https://github.com/aleoli/Shared-Editor/blob/master/docs/images/cs_comm/new_dir.png?raw=true)



## remove_dir

![](https://github.com/aleoli/Shared-Editor/blob/master/docs/images/cs_comm/remove_dir.png?raw=true)



## rename_dir

![](https://github.com/aleoli/Shared-Editor/blob/master/docs/images/cs_comm/rename_dir.png?raw=true)



## list_files

![](https://github.com/aleoli/Shared-Editor/blob/master/docs/images/cs_comm/ls_files.png?raw=true)

> Se dir_id è `NULL` allora restituisco la root dell'utente

> La `FileList` contiene sia file che directory, si deve fare una classe da cui entrambi derivano (es. `FS_Item`), o anche solo inviare JSON del tipo
```json
[
 {
  "id": 1,
  "is_dir": false,
  "name": "file1",
  ...
 },
 {
  "id": 1,
  "is_dir": true,
  "name": "dir1"
 }
]
```



## new_file

![](https://github.com/aleoli/Shared-Editor/blob/master/docs/images/cs_comm/new_file.png?raw=true)



## get_file

![](https://github.com/aleoli/Shared-Editor/blob/master/docs/images/cs_comm/get_file.png?raw=true)



## mod_file

![](https://github.com/aleoli/Shared-Editor/blob/master/docs/images/cs_comm/mod_file.png?raw=true)

Ci sono 2 tipi di comunicazione, quella del client verso il server e quella del server verso il client

Da vedere se si vuole utilizzare o no un messaggio di `ACK`

> Si può fare che venga sempre inviato un array di Symbol, in questo modo si possono anche inserire più simboli contemporaneamente con lo stesso messaggio



## close_file

![](https://github.com/aleoli/Shared-Editor/blob/master/docs/images/cs_comm/close_file.png?raw=true)

Indica al server che non si sta più lavorando su quel file



## rename_file

![](https://github.com/aleoli/Shared-Editor/blob/master/docs/images/cs_comm/rename_file.png?raw=true)

Indica al server che l'utente vuole modificare il nome del file



## move_file

![](https://github.com/aleoli/Shared-Editor/blob/master/docs/images/cs_comm/move_file.png?raw=true)



## remove_file

![](https://github.com/aleoli/Shared-Editor/blob/master/docs/images/cs_comm/remove_file.png?raw=true)



## share_file

![](https://github.com/aleoli/Shared-Editor/blob/master/docs/images/cs_comm/share_file.png?raw=true)

Bisogna valutare la durata del link e se renderlo pubblico a chiunque riceva il link o collegarlo a dei nomi utente

> In base a questo c'è da costruire un messaggio per collegarsi ad un file condiviso



## link_file

Dipende dalle scelte fatte nel `share_file`



## chg_nickname

![](https://github.com/aleoli/Shared-Editor/blob/master/docs/images/cs_comm/chg_nickname.png?raw=true)



## chg_icon

![](https://github.com/aleoli/Shared-Editor/blob/master/docs/images/cs_comm/chg_icon.png?raw=true)

L'icona (PNG o JPEG) deve avere dimensioni ridotte e va inviata all'interno del JSON come stringa in base64, quando arriva sul server viene salvata

Quando serve rimanda indietro la stringa e il client la decodifica e la visualizza

[esempio](https://stackoverflow.com/questions/27854979/how-to-load-base64-image-data-from-json-in-qt)



## chg_pass

![](https://github.com/aleoli/Shared-Editor/blob/master/docs/images/cs_comm/chg_pass.png?raw=true)







***to be continued...***

