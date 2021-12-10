#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QWidget>
#include <QAuthenticator>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QByteArray>
#include <QBuffer>
#include <QDomDocument>
#include "calendar.h"

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = nullptr);
    void addCalendar();

    const int getCalendarCount() const;

    void setCalendarList(const QList<Calendar *> &newCalendarList);
    QList<Event*> getEventByDate(const QDate &date);
    const QList<Calendar *> &calendarList() const;

    QList<Todo*> getTodoByDate(const QDate &date);
public slots:
    void handleAddCalendar(QString username, QString password, QString url);
    void handleAddCalendarError(QString errorMessage);

    void handleAddEventError(QString errorMessage);
    void handleRemoveCalendarFromList(Calendar* cal);

signals:
    void requestSyncToken(void);
    void dialogErrorMessage(QString errorMessage);
    void closeDialog();

    void eventDialogErrorMessage(QString errorMessage);
    void closeEventDialog(Event* event);

    void printEvent(QList<Event*> eventList);

    void updateMainWindow();

private:
    QList<Calendar*> _calendarList;
    QList<QString> _colourList = { "#ef5350", "#ab47bc", "#42a5f5", "#26a69a", "#d4e157", "#ffa726", "#bdbdbd"};


private slots:
    void handleAddCalendarFinished();



};

#endif // CLIENT_H
