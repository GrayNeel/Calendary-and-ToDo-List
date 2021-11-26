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

signals:
    void calendarRetrieveError(QString errorMessage);
    void calendarAdded();

public slots:

private slots:
    void APIRequestSyncToken(void);
    void handleAuthentication(QNetworkReply *reply, QAuthenticator *authenticator) const;
    void checkResponseStatus();
    void handleRequestSyncTokenFinished();
    void APIRequestVCalendarObjects(void);
    void handleRequestVCalendarObjectsFinished();

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

    QList<Event*> _eventsList;
    QList<Todo*> _todosList;
};

#endif // CALENDAR_H
