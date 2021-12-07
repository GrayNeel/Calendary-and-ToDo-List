#include "client.h"

Client::Client(QObject *parent) : QObject(parent)
{
    //connect(, eventAddCalendar, this, handleAddCalendar)
}

void Client::handleAddCalendar(QString username, QString password, QString url)
{
//    qDebug() << "handleAddCalendar:" + username + " " + password + " " + url;

    Calendar* cal = new Calendar(this);
    connect(cal,&Calendar::calendarRetrieveError,this,&Client::handleAddCalendarError);
    connect(cal,&Calendar::calendarAdded, this, &Client::handleAddCalendarFinished);

    _calendarList.append(cal);
    cal->setUsername(username);
    cal->setPassword(password);

    if(url.back() != QChar('/')) {
        cal->setUrl(url.append('/'));
    }else{
        cal->setUrl(url);
    }

    emit requestSyncToken();

    //delete cal;
}

void Client::handleAddCalendarError(QString errorMessage) {
    qDebug() << "HandleAddCalendarError: " << errorMessage;
    Calendar* cal = _calendarList.last();
    _calendarList.removeLast();

    disconnect(cal,&Calendar::calendarRetrieveError,this,&Client::handleAddCalendarError);
    disconnect(cal,&Calendar::calendarAdded, this, &Client::handleAddCalendarFinished);

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
