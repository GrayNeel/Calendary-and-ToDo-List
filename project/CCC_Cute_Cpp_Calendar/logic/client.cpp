#include "client.h"

Client::Client(QObject *parent) : QObject(parent)
{
    //connect(, eventAddCalendar, this, handleAddCalendar)
    connect(this, SIGNAL(requestSyncToken()), this, SLOT(handleRequestSyncToken()));
}

void Client::handleAddCalendar(QString username, QString password, QString url)
{
    qDebug() << "handleAddCalendar:" + username + " " + password + " " + url;

    // Saving data to let the Authenticator use them if needed
    _username = username;
    _password = password;
    _url = url;

    emit requestSyncToken();
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

void Client::handleAuthentication(QNetworkReply *reply, QAuthenticator *q) const{
    q->setUser(_username);
    q->setPassword(_password);
}

void Client::checkResponseStatus() {
    int statusCode = _reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if(statusCode == 0)
        qDebug() << "Bad URL";

    if (statusCode >= 200 && statusCode < 300)
        qDebug() << "Good Response: " << statusCode;
    else
        qDebug() << "Bad Response: " << statusCode;
}

void Client::handleRequestSyncToken(void) {
    QNetworkRequest request;

    // Building the header of a Sync-Token PROPFIND request

    request.setUrl(QUrl(_url));
    request.setAttribute(QNetworkRequest::Http2AllowedAttribute, false); // Fallback to HTTP 1.1
    request.setRawHeader("Depth", "0");
    request.setRawHeader("Content-Type", "application/xml; charset=utf-8");

    // Building the Body

    QString requestString = "<d:propfind xmlns:d=\"DAV:\" xmlns:cs=\"http://calendarserver.org/ns/\">\r\n"
                              "  <d:prop>\r\n"
                              "    <d:displayname />\r\n"
                              "    <cs:getctag />\r\n"
                              "    <d:sync-token />"
                              "  </d:prop>\r\n"
                              "</d:propfind>";

    QBuffer* buffer = new QBuffer();
    buffer->open(QIODevice::ReadWrite);

    int bufferSize = buffer->write(requestString.toUtf8());
    buffer->seek(0);

    QByteArray contentLength;
    contentLength.append(QString::number(bufferSize).toStdString());

    request.setRawHeader("Content-Length", contentLength);

//    request.setRawHeader("Host", "127.0.0.1:8080");
//    request.setRawHeader("User-Agent", "Cute_Cpp_Calendar_Client");
//    request.setRawHeader("Prefer", "return-minimal");
//    request.setRawHeader("Content-Length", 0);

    _reply = _manager->sendCustomRequest(request, QByteArray("PROPFIND"), buffer);

    // If authentication is required, provide credentials
    connect(_manager, &QNetworkAccessManager::authenticationRequired, this, &Client::handleAuthentication);

    // When request ends check the status (200 OK or not) and then handle the Reply
    connect(_reply, SIGNAL(finished()), this, SLOT(checkResponseStatus()));
    connect(_reply, SIGNAL(finished()), this, SLOT(handleRequestSyncTokenFinished()));
}

void Client::handleRequestSyncTokenFinished(void) const {
    qDebug() << "RequestSyncTokenFinished";
}
