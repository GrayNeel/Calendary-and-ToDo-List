#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
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

public slots:
    void handleAddCalendar(QString username, QString password, QString url);
signals:
    void requestSyncToken(void);

private:
    QList<Calendar*> _calendarList;
    QNetworkReply *_reply;
    QNetworkAccessManager *_manager = new QNetworkAccessManager(this);
    QString _username;
    QString _password;
    QString _url;
    QString _replyStatus;

private slots:
    void handleRequestSyncToken(void);
    void checkResponseStatus();
    void handleAuthentication(QNetworkReply *reply, QAuthenticator *authenticator) const;
    void handleRequestSyncTokenFinished() const;
};

#endif // CLIENT_H
