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

signals:
    void calendarRetrieveError(QString errorMessage);
    void calendarAdded();

public slots:

private slots:
    void APIRequestSyncToken(void);
    void handleAuthentication(QNetworkReply *reply, QAuthenticator *authenticator) const;
    void checkResponseStatus();
    void handleRequestSyncTokenFinished();

private:
    QString _username;
    QString _password;
    QString _url;
    QNetworkReply *_reply;
    QNetworkAccessManager *_manager;
    QString _replyStatus;
    int _statusCode;
};

#endif // CALENDAR_H
