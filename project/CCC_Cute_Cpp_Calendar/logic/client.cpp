#include "client.h"

Client::Client(QObject *parent) : QObject(parent)
{
    //connect(, eventAddCalendar, this, handleAddCalendar)
}

void Client::handleAddCalendar(QString username, QString password, QString url)
{
    qDebug("ciao ");
    qDebug()<<password + " " + url;

    this->username = username;
    this->password = password;
    QBuffer* buffer = new QBuffer();
    buffer->open(QIODevice::ReadWrite);
    buffer->write("");

    QNetworkRequest request;
    request.setAttribute(QNetworkRequest::Http2AllowedAttribute, false);
    request.setUrl(QUrl(url));
    request.setRawHeader("Host", "127.0.0.1:8080");
    request.setRawHeader("User-Agent", "Cute_Cpp_Calendar_Client");
    request.setRawHeader("Depth", "0");
    request.setRawHeader("Prefer", "return-minimal");
    request.setRawHeader("Content-Type", "text/xml; charset=utf-8");
    request.setRawHeader("Content-Length", 0);


    reply = manager->sendCustomRequest(request, QByteArray("GET"), buffer);
    connect(reply, SIGNAL(finished()), this, SLOT(handleHTTPError()));
    connect(manager, &QNetworkAccessManager::authenticationRequired, this, &Client::handleAuth);
}


void Client::handleAuth(QNetworkReply *reply, QAuthenticator *q) const{
    q->setUser(username);
    q->setPassword(password);
}

void Client::handleHTTPError() const {
    //if(reply->rawHeader("Server"))
//    qDebug() << QString(reply->rawHeader("Server"));
}
