#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QAuthenticator>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QBuffer>
#include "calendar.h"


class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = nullptr);
    void addCalendar();
private:
    QList<Calendar*> _calendarList;
    QNetworkReply *reply;
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QString username;
    QString password;

signals:

private slots:

    void handleAuth(QNetworkReply *reply, QAuthenticator *authenticator) const;
    void handleHTTPError() const;
public slots:
    void handleAddCalendar(QString username, QString password, QString url);
};

#endif // CLIENT_H
