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
    cal->setUrl(url);

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
}

//TODO: implement this
void Client::handleAddEvent(QString summary, QString location, QString description, QDateTime startDateTime, QDateTime endDateTime) {

}

//TODO: implement this
void Client::handleAddEventError(QString errorMessage) {

}
