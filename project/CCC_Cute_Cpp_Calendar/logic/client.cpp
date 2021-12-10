#include "client.h"

Client::Client(QObject *parent) : QObject(parent)
{
    //connect(, eventAddCalendar, this, handleAddCalendar)
}

void Client::handleAddCalendar(QString username, QString password, QString url)
{
//    qDebug() << "handleAddCalendar:" + username + " " + password + " " + url;

    // Create in advance the calendar object. If there will be any error it will be removed.
    Calendar* cal = new Calendar(this);
    connect(cal,&Calendar::calendarRetrieveError,this,&Client::handleAddCalendarError);
    connect(cal,&Calendar::calendarAdded, this, &Client::handleAddCalendarFinished);
    connect(cal, &Calendar::refreshEventVisualization, this, &Client::handleRefreshEventVisualization);

    _calendarList.append(cal);
    cal->setUsername(username);
    cal->setPassword(password);

    //simple assigning colour property from a list of possible colour
    QString first = _colourList.first();
    cal->setColour(first);
    _colourList.removeFirst();
    _colourList.append(first);

    if(url.back() != QChar('/')) {
        cal->setUrl(url.append('/'));
    }else{
        cal->setUrl(url);
    }

    emit requestSyncToken();
}

void Client::handleAddCalendarError(QString errorMessage) {
    qDebug() << "HandleAddCalendarError: " << errorMessage;
    Calendar* cal = _calendarList.last();
    _calendarList.removeLast();

    disconnect(cal,&Calendar::calendarRetrieveError,this,&Client::handleAddCalendarError);
    disconnect(cal,&Calendar::calendarAdded, this, &Client::handleAddCalendarFinished);
    disconnect(cal, &Calendar::refreshEventVisualization, this, &Client::handleRefreshEventVisualization);

    delete cal;

    emit dialogErrorMessage(errorMessage);
}

const int Client::getCalendarCount() const
{
    return _calendarList.count();
}

void Client::handleAddCalendarFinished() {
    Calendar* cal = _calendarList.last();

    disconnect(cal,&Calendar::calendarRetrieveError,this,&Client::handleAddCalendarError);
    disconnect(cal,&Calendar::calendarAdded, this, &Client::handleAddCalendarFinished);

    emit closeDialog(cal);
    emit printEvent(cal->eventsList());
}

void Client::handleRemoveCalendarFromList(Calendar* cal){
    _calendarList.removeOne(cal);
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
            qDebug() << ev->startDateTime().toString();
            if(ev->startDateTime().date() == date) {
                eventsList.append(ev);
            }
        }
    }

    return eventsList;
}

void Client::handleRefreshEventVisualization(){
    emit refreshEventVisualization();
}

const QList<Calendar *> &Client::calendarList() const
{
    return _calendarList;
}
