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

void Client::handleAddCalendarFinished() {
    Calendar* cal = _calendarList.last();

    disconnect(cal,&Calendar::calendarRetrieveError,this,&Client::handleAddCalendarError);
    disconnect(cal,&Calendar::calendarAdded, this, &Client::handleAddCalendarFinished);

    emit closeDialog();
}

//void Client::handleAddCalendar(QString username, QString password, QString url)
//{
//    Calendar cal = new Calendar();
//    _calendars.put(cal);
//    conncet(cal, SIGNAL(uccidimi()), this, SLOT(tuttoMale()))
//    connect (this, cal)
//    emit initializeCalendar(); //nello slot della clase Calendar associato a initialize, fa la PROPFIND per il token e i controlli ecc (se tutto ok prende gli eventi)
//}

// funzione tuttoMale() {
// delete cal
// }
