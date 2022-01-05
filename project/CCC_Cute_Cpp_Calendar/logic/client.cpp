#include "client.h"

Client::Client(QObject *parent) : QObject(parent)
{

}

void Client::handleAddCalendar(QString username, QString password, QString url)
{
    // Adjust link
    if(url.back() != QChar('/')) {
        url = url.append('/');
    }

    for (Calendar *cal : _calendarList) {
        if(url == cal->url()) {
            emit dialogErrorMessage("Il calendario richiesto è già stato inserito.");
            return;
        }
    }
    // Create in advance the calendar object. If there will be any error it will be removed.
    Calendar* cal = new Calendar(this);

    connect(cal,&Calendar::calendarRetrieveError,this,&Client::handleAddCalendarError);
    connect(cal,&Calendar::calendarAdded, this, &Client::handleAddCalendarFinished);
    connect(cal, SIGNAL(refreshEventVisualization()), this->parent(), SLOT(handleUpdateMainWindowWidgets()));
    connect(cal,&Calendar::refreshLocalCalendarData,this,&Client::handleRefreshCalendar);

    _calendarList.append(cal);
    cal->setUsername(username);
    cal->setPassword(password);
    cal->setUrl(url);

    //simple assigning colour property from a list of possible colour
    QString first = _colourList.first();
    cal->setColour(first);
    _colourList.removeFirst();
    _colourList.append(first);

    emit requestSyncToken();
}

void Client::handleAddCalendarError(QString errorMessage, QString url) {
    qDebug() << "HandleAddCalendarError: " << errorMessage;
    //Calendar* cal = _calendarList.last();
    Calendar* cal;
    for(Calendar* calendar: _calendarList)
        if(calendar->url() == url)
            cal = calendar;
    _calendarList.removeOne(cal);
    //_calendarList.removeLast();

    disconnect(cal,&Calendar::calendarRetrieveError,this,&Client::handleAddCalendarError);
    disconnect(cal,&Calendar::calendarAdded, this, &Client::handleAddCalendarFinished);
    disconnect(cal, SIGNAL(refreshEventVisualization()), this->parent(), SLOT(handleUpdateMainWindowWidgets()));

    delete cal;

    emit dialogErrorMessage(errorMessage);
}

const int Client::getCalendarCount() const
{
    return _calendarList.count();
}

void Client::handleAddCalendarFinished(QString addedUrl) {
    //Calendar* cal = _calendarList.last();

    Calendar* cal;
    for(Calendar* calendar: _calendarList)
        if(calendar->url() == addedUrl)
            cal = calendar;

    disconnect(cal,&Calendar::calendarRetrieveError,this,&Client::handleAddCalendarError);
    disconnect(cal,&Calendar::calendarAdded, this, &Client::handleAddCalendarFinished);

    emit closeDialog();
    emit updateMainWindow();
}

void Client::handleRemoveCalendarFromList(Calendar* cal){
    _calendarList.removeOne(cal);
    cal->eraseEventsTodos();
    delete cal;
    emit updateMainWindow();
    // qDebug() << "Sono lo slot dentro client";
}



//TODO: implement this
void Client::handleAddEventError(QString errorMessage) {

}

QList<Event*> Client::getEventByDate(const QDate &date) {
    QList<Event*> eventsList;

    for (Calendar* cal : _calendarList) {
        for (Event* ev : cal->eventsList()) {
//            qDebug() << ev->startDateTime().toString();
            if(ev->startDateTime().date() <= date && ev->endDateTime().date() >= date) {
                eventsList.append(ev);
            }
        }
    }

    return eventsList;
}

QList<Todo*> Client::getTodoByDate(const QDate &date) {
    QList<Todo*> todosList;

    for (Calendar* cal : _calendarList) {
        for (Todo* td : cal->todosList()) {
            qDebug() << td->dueDateTime().toString();
            if(td->dueDateTime().date() == date) {
                todosList.append(td);
            }
        }
    }

    return todosList;
}

const QList<Calendar *> &Client::calendarList() const
{
    return _calendarList;
}

void Client::handleRefreshCalendars() {
    QList<Calendar*> oldCalendarList(_calendarList);

    _calendarList.clear();

    for(Calendar* cal : oldCalendarList) {
        handleRefreshCalendar(cal);
    }

}


void Client::handleRefreshCalendar(Calendar* cal) {
    QString username = cal->username();
    QString password = cal->password();
    QString url = cal->url();

    cal->eraseEventsTodos();
    cal->blockSignals(true);

    //Prima versione di aggiornamento di un calendario locale
    //Versioni ottimizzate prevedono l'aggiornamento della lista senza la rimozione dell'oggetto calendario, non potendo riutilizzare però l'api di aggiunta di un nuovo calendario
    //handleAddCalendar->synctoken...
    _calendarList.removeOne(cal);
    delete cal;

    handleAddCalendar(username,password,url);
}

