# Applicazione Calendario e To-Do List all-in-one
Progetto di Programmazione di Sistema: Applicazione Calendario e To-Do List all-in-one. Made by @lelexdrugo &amp; @GrayNeel

## Definizione del problema
CalDAV è uno standard aperto per la sincronizzazione di eventi di calendario e liste di task (o todo
list). La difficoltà nella gestione dello standard è l’adattabilità dei SW correntemente disponibili
a fornire un supporto integrato che permetta, ad esempio, di poter visualizzare in un unico posto
eventi (come le lezioni) e to-do (eventualmente congruentemente con date ed orari di scadenza).

## Obiettivi
L’obiettivo primario del progetto è sviluppare una applicazione in grado di visualizzare ed interagire
con un server CalDAV (es: sabre/dav, Nextcloud, ecc), fornendo un’interfaccia grafica che
supporti l’integrazione di entrambe le parti (calendario e to-do list) nello stesso spazio. L’applicazione
potrà basarsi su una qualsiasi libreria GUI C++ di terze parti (es: QT, JUCE, FLTK, ecc.).
Unico vincolo insindacabile allo sviluppo del progetto è l’uso di CMake per tutta la catena di deployment.  

Come obiettivi secondari, si richiede agli sviluppatori di porre la massima attenzione alle necessità
di asincronia e parallelizzazione del problema. A fronte, infatti, di una interazione utente con calendari
e task (creazione/modifica/cancellazione degli stessi) la natura del problema richiede la
necessità di sincronizzare le informazioni con il server al fine di supportare, tra le altre possibilità,
la condivisione del singolo calendario/task list tra più utenti.

###### Referenze
https://tools.ietf.org/html/rfc6638

# Installazione di sabre/dav (Windows)

Per l'installazione di sabre/dav è necessaria l'installazione e la configurazione dei seguenti applicativi:

- Server Apache 2.4
- PHP v7.3.31 (versioni superiori creano problemi)
- Sabre/Dav 3.2.2

## Configurazione di PHP

Una volta scaricato lo ZIP di PHP dal sito ufficiale, bisogna inserirlo nella radice del file system (ad esempio in `C:\`) e rinominare la cartella in `php`.

Successivamente è necessario aprire la cartella e rinominare il file `php.ini-development` in `php.ini` e aprirlo con un qualsiasi editor di testo. Bisogna abilitare (e dunque togliere il `;` nelle corrispettive righe del file) le seguenti estensioni:

- `extension=mbstring`
- `extension=sqlite3`
- `extension=pdo_sqlite`

Queste estensioni permetteranno successivamente a sabre/dav di avviarsi correttamente.

## Configurazione di Apache

Sabre/DAV va installato su un webserver funzionante. Per questo progetto è stato scelto Apache, perciò, una volta installato è necessario effettuare le seguenti operazioni.

Raggiungere la cartella `Apache24/conf/` e aprire il file `httpd.conf`. Questo è il file di configurazione di Apache a cui effettuare le seguenti modifiche:

1. Rimuovere il commento alla linea di codice seguente: `LoadModule rewrite_module modules/mod_rewrite.so`. Questo modulo serve per indirizzare tutte le richieste HTTP al nostro server sabre/dav.
2. Aggiungere le seguenti righe di codice avendo cura di inserire l'esatto path in cui si trova la cartella di PHP e il relativo file dll: 
```
AddHandler application/x-httpd-php .php 
AddType application/x-httpd-php .php .html 
LoadModule php7_module "C:/your/path/to/php/php7apache2_4.dll"
PHPIniDir "C:/your/path/to/php"
``` 

3. Rimuovere il commento alla seguente linea di codice: `Include conf/extra/httpd-vhosts.conf`. 

Il passaggio 3 abilita un ulteriore file di configurazione necessario per attivare le VirtualHost (cioè per permettere ad Apache di gestire il webserver dedicato a sabre/dav). Dunque bisogna recarsi in `Apache24/conf/extra` e aprire il file `httpd-vhosts.conf`. Inserire la seguente virtualhost:
```
<VirtualHost *:8080>
    ServerAdmin 127.0.0.1
    ServerName 127.0.0.1
    DocumentRoot "C:/path/to/the/project/server"
    <Directory "C:/path/to/the/project/server">
        #
        # Possible values for the Options directive are "None", "All",
        # or any combination of:
        #   Indexes Includes FollowSymLinks SymLinksifOwnerMatch ExecCGI MultiViews
        #
        # Note that "MultiViews" must be named *explicitly* --- "Options All"
        # doesn't give it to you.
        #
        # The Options directive is both complicated and important.  Please see
        # http://httpd.apache.org/docs/2.4/mod/core.html#options
        # for more information.
        #
        Options Indexes FollowSymLinks

        #
        # AllowOverride controls what directives may be placed in .htaccess files.
        # It can be "All", "None", or any combination of the keywords:
        #   Options FileInfo AuthConfig Limit
        #
        AllowOverride None

        #
        # Controls who can get stuff from this server.
        #
        Require all granted
    </Directory>
    
    RewriteEngine On
    # This makes every request go to server.php
    RewriteRule ^/(.*)$ /server.php [L]
    # Output buffering needs to be off, to prevent high memory usage
    php_flag output_buffering off

    # This is also to prevent high memory usage
    php_flag always_populate_raw_post_data off

    # This is almost a given, but magic quotes is *still* on on some
    # linux distributions
    php_flag magic_quotes_gpc off

    # SabreDAV is not compatible with mbstring function overloading
    php_flag mbstring.func_overload off


    # ServerAlias www.dummy-host.example.com
    # ErrorLog "logs/dummy-host.example.com-error.log"
    # CustomLog "logs/dummy-host.example.com-access.log" common
</VirtualHost>
```

È necessario indicare in `DocumentRoot` il path in cui si trova il server sabre/dav (che nel nostro caso è all'interno della cartella Server) e specificarlo nuovamente nella direttiva della Directory.
Le righe di codice menzionate sopra abilitano il server in ascolto all'indirizzo `127.0.0.1:8080` ed effettuano il redirect di tutte le richieste alla pagina PHP `server.php` (tramite la RewriteRule) contenuto nella directory specificata in precedenza.

## Avvio del server

Una volta completate tutte le modifiche citate, è necessario riavviare Apache (Per Windows: `Gestione attività -> Servizi -> Apache -> Riavvia`) e provare a collegarsi a `127.0.0.1:8080`. Se tutto è andato a buon fine, dovrebbe comparire una richiesta da parte del server di un username e di una password, specificati di seguito.

## Utenti del Calendario

Il calendario è popolato dei seguenti utenti:

| Nome     | Password |
| -----    | -------- |
| admin    |  admin   |
| marco    |  marco   |
| gabriele | gabriele |

Gli utenti vengono generati "a priori" (dunque vanno scritti nel database a mano) poiché sabre/dav non fornisce alcuna API per la creazione degli utenti.