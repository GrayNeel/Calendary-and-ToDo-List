#ifndef CALENDAR_H
#define CALENDAR_H

#include <QObject>
#include <QAuthenticator>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QByteArray>
#include <QBuffer>
#include <QDomDocument>

#include "event.h"
#include "todo.h"

class Calendar : public QObject
{
    Q_OBJECT
public:
    explicit Calendar(QObject *parent = nullptr);
    ~Calendar();

    const QString &username() const;
    void setUsername(const QString &newUsername);
    const QString &password() const;
    void setPassword(const QString &newPassword);
    const QString &url() const;
    void setUrl(const QString &newUrl);

    const QString &displayName() const;

    const QList<Event *> &eventsList() const;

    const QString &colour() const;
    void setColour(const QString &newColour);

    const QList<Todo *> &todosList() const;

signals:
    void calendarRetrieveError(QString errorMessage);
    void calendarAdded();
    void removeCalendar(Calendar* cal);
    void showEventDialog(Calendar* cal);
    void eventAddFinished();
    void eventRetrieveError();
    void refreshEventVisualization();
    void showTodoDialog(Calendar* cal);
    void todoAddFinished();
    void todoRetrieveError();
    void eventModifyFinished();
    void eventModifyRetrieveError();
    void todoModifyFinished();
    void todoModifyRetrieveError();

public slots:
    void handleAddEvent(QString summary, QString location, QString description, QDateTime startDateTime, QDateTime endDateTime);
    void handleAddTodo(QString summary, QDateTime dueDateTime);

    void handleModifyEvent(QString oldUid, QString summary, QString location, QString description, QDateTime startDateTime, QDateTime endDateTime);
    void handleModifyTodo(QString oldUid, QString summary, QDateTime dueDateTime);

    void deleteEvent(Event *event);
    void deleteTodo(Todo *todo);

private slots:
    void APIRequestSyncToken(void);
    void handleAuthentication(QNetworkReply *reply, QAuthenticator *authenticator) const;
    void checkResponseStatus();
    void handleRequestSyncTokenFinished();
    void APIRequestVCalendarObjects(void);
    void handleRequestVCalendarObjectsFinished();
    void handleRemoveCalendar();
    void handleAddNewEventPopUp();
    void handleAddNewTodoPopUp();
    void handleDeletingVEventFinished();

    void handleGetVEventFinished();
    void handleGetTodoFinished();


    void APIAddEvent(Event *event);
    void handleAddingVEventFinished();
    void handleDeletingVTodoFinished();
    void handleAddingVTodoFinished();

    void APIUpdateEvent(Event* event, QString oldEtag);
    void APIUpdateTodo(Todo* todo, QString oldEtag);
private:
    QString _username;
    QString _password;
    QString _url;
    QNetworkReply *_reply;
    QNetworkAccessManager *_manager;
    QString _replyStatus;
    int _statusCode;

    QString _displayName;
    QString _cTag;
    QString _syncToken;

    QString _colour;

    QList<Event*> _eventsList;
    QList<Todo*> _todosList;
    void getForLastVEventResource(QUrl resourceUrl);
    void getForLastTodoResource(QUrl resourceUrl);
    void parseResponse();
    void parseCalendarData(QString entity, QString uri, QString etag);
    void APIAddTodo(Todo *todo);
};

#endif // CALENDAR_H
