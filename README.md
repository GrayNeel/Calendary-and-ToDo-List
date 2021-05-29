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

# Referenze
https://tools.ietf.org/html/rfc6638
