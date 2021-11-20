#include "calendar.h"

Calendar::Calendar(QObject *parent) : QObject(parent)
{
    // Create a QNetworkAccessManager that will be unique for this specific calendar
    _manager = new QNetworkAccessManager(this);

    connect(parent,SIGNAL(requestSyncToken()),this,SLOT(APIRequestSyncToken()));
}

Calendar::~Calendar(void) {
    delete _manager;
}

const QString &Calendar::username() const
{
    return _username;
}

void Calendar::setUsername(const QString &newUsername)
{
    _username = newUsername;
}

const QString &Calendar::password() const
{
    return _password;
}

void Calendar::setPassword(const QString &newPassword)
{
    _password = newPassword;
}

const QString &Calendar::url() const
{
    return _url;
}

void Calendar::setUrl(const QString &newUrl)
{
    _url = newUrl;
}

/*************************** API Requests to CalDAV Server ****************************/

/**
 * @brief This API Requests a Sync Token to the server
 */
void Calendar::APIRequestSyncToken(void) {
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

    // When request ends check the status (200 OK or not) and then handle the Reply
    connect(_reply, SIGNAL(finished()), this, SLOT(checkResponseStatus()));
    connect(_reply, SIGNAL(finished()), this, SLOT(handleRequestSyncTokenFinished()));
    // If authentication is required, provide credentials
    connect(_manager, &QNetworkAccessManager::authenticationRequired, this, &Calendar::handleAuthentication);

}

void Calendar::handleAuthentication(QNetworkReply *reply, QAuthenticator *q) const {
    q->setUser(_username);
    q->setPassword(_password);
}

void Calendar::checkResponseStatus() {
    _statusCode = _reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if(_statusCode == 0) {
//        qDebug() << "Bad URL";
        emit calendarRetrieveError(QString("Wrong URL. Try again!"));
    }

    if (_statusCode >= 200 && _statusCode < 300) {
//        qDebug() << "Good Response: " << statusCode;

    } else {
//        qDebug() << "Bad Response: " << statusCode;
        emit calendarRetrieveError(QString("Wrong credentials. Try again!"));
    }
}

void Calendar::handleRequestSyncTokenFinished(void) {
    qDebug() << "RequestSyncTokenFinished";
    if(_statusCode >= 200 && _statusCode < 300) {
        QDomDocument doc;

        doc.setContent(_reply);

        QDomNodeList response = doc.elementsByTagName("d:prop");
        for (int i = 0; i < response.size(); i++) {
            QDomNode n = response.item(i);
            QDomElement displayname = n.firstChildElement("d:displayname");
            if (!displayname.isNull())
                _displayName = displayname.text();

            QDomElement ctag = n.firstChildElement("cs:getctag");
            if (!ctag.isNull())
                _cTag = ctag.text();

            QDomElement syncToken = n.firstChildElement("d:sync-token");
            if (!syncToken.isNull())
                _syncToken = syncToken.text();
        }

        emit calendarAdded();
    }

}
