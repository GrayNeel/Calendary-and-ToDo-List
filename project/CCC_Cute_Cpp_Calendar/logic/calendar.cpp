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

const QString &Calendar::displayName() const
{
    return _displayName;
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
    request.setRawHeader("Prefer", "return-minimal");
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
        qDebug() << "Bad URL";
        emit calendarRetrieveError(QString("Wrong URL. Try again!"));
    }

    if (_statusCode >= 200 && _statusCode < 300) {
        qDebug() << "Good Response: " << _statusCode;
        qDebug() << "Evento aggiunto correttamente";

    } else {
        qDebug() << "Bad Response: " << _statusCode;
        emit calendarRetrieveError(QString("Wrong credentials. Try again!"));
    }
}

void Calendar::handleRequestSyncTokenFinished(void) {
//    qDebug() << "RequestSyncTokenFinished";
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

        //If i'm here everything is ok and I will start downloading VCalObjects
        APIRequestVCalendarObjects();
    }

}

/**
 * @brief This API Requests the list of VCalendar Objects from a specific fetched calendar
 */
void Calendar::APIRequestVCalendarObjects(void) {
    QNetworkRequest request;

    // Building the header of a REPORT request to get VCalendar Objects

    request.setUrl(QUrl(_url));
    request.setAttribute(QNetworkRequest::Http2AllowedAttribute, false); // Fallback to HTTP 1.1
    request.setRawHeader("Depth", "1");
    request.setRawHeader("Prefer", "return-minimal");
    request.setRawHeader("Content-Type", "application/xml; charset=utf-8");

    // Building the Body

    QString requestString = "<c:calendar-query xmlns:d=\"DAV:\" xmlns:c=\"urn:ietf:params:xml:ns:caldav\">\r\n"
                                "<d:prop>\r\n"
                                    "<d:getetag />\r\n"
                                    "<c:calendar-data />\r\n"
                                "</d:prop>\r\n"
                                "<c:filter>\r\n"
                                    "<c:comp-filter name=\"VCALENDAR\" />\r\n"
                                "</c:filter>\r\n"
                           "</c:calendar-query>\r\n";

    QBuffer* buffer = new QBuffer();
    buffer->open(QIODevice::ReadWrite);

    int bufferSize = buffer->write(requestString.toUtf8());
    buffer->seek(0);

    QByteArray contentLength;
    contentLength.append(QString::number(bufferSize).toStdString());

    request.setRawHeader("Content-Length", contentLength);

    _reply = _manager->sendCustomRequest(request, QByteArray("REPORT"), buffer);

    // When request ends check the status (200 OK or not) and then handle the Reply
    connect(_reply, SIGNAL(finished()), this, SLOT(checkResponseStatus()));
    connect(_reply, SIGNAL(finished()), this, SLOT(handleRequestVCalendarObjectsFinished()));
    // If authentication is required, provide credentials
    connect(_manager, &QNetworkAccessManager::authenticationRequired, this, &Calendar::handleAuthentication);

}

/**
 * TODO: Manage the fetched VCalObjects and add events/todos to list
 */
void Calendar::handleRequestVCalendarObjectsFinished(void) {
    qDebug() << "requestVCalendarObjectsFinished";
    if(_statusCode >= 200 && _statusCode < 300) {
        emit calendarAdded();
    }
}


void Calendar::handleRemoveCalendar(){
    // qDebug() << "Sono lo slot dentro calendar";
    emit removeCalendar(this);
}

void Calendar::handleAddNewEventPopUp() {
    emit showEventDialog(this);
}

void Calendar::handleAddEvent(QString summary, QString location, QString description, QDateTime startDateTime, QDateTime endDateTime) {
    //if uid=-1 I have a new event. Constructor will choose a new one based on timestamp
    //if filename is empty constructor create it basing on uid
    //update API is based on a specific uid
    Event* newEvent = new Event(QString("-1"), QString(""), summary, location, description, QString(""), QString(""), startDateTime, endDateTime);
    _eventsList.append(newEvent);

    APIAddEvent(newEvent);
}

/**
 *
 * @brief This API push a new event in a VCalendar Object from a specific calendar
 */
void Calendar::APIAddEvent(Event* event) {
    // https://datatracker.ietf.org/doc/html/rfc4791#section-5.3.2
    QNetworkRequest request;

    // Building the header of a PUT request to push a VCalendar Object for ONE VEVENT

    request.setUrl(QUrl(_url + event->filename()));
    request.setAttribute(QNetworkRequest::Http2AllowedAttribute, false); // Fallback to HTTP 1.1
    //"The "If-None-Match: *" request header ensures that the client will not inadvertently overwrite an existing resource
    //if the last path segment turned out to already be used"
    request.setRawHeader("If-None-Match", "*");
    request.setRawHeader("Content-Type", "text/calendar; charset=utf-8");

    // Building the Body

    QString requestString = "BEGIN:VCALENDAR\r\n"
                            "VERSION:2.0\r\n"
                            "BEGIN:VEVENT\r\n"
                            "UID:" + event->uid() + "\r\n"
                            "DTSTAMP:" + QDateTime::currentDateTime().toString("yyyyMMddTHHmmssZ") + "\r\n"
                            "DTSTART:" + event->startDateTime().toString("yyyyMMddTHHmmss") + "\r\n"
                            "DTEND:" + event->endDateTime().toString("yyyyMMddTHHmmss") + "\r\n"
                            "SUMMARY:" + event->summary() + "\r\n"
                            "LOCATION:" + event->location() + "\r\n"
                            "DESCRIPTION:" + event->description() + "\r\n";
    if (!event->rrule().isEmpty())
    {
      requestString.append("RRULE:" + event->rrule() + "\r\n");
    }

    if (!event->exdate().isEmpty())
    {
      requestString.append("EXDATE:" + event->exdate() + "\r\n");
    }

    requestString.append("END:VEVENT\r\nEND:VCALENDAR");

    QBuffer* buffer = new QBuffer();
    buffer->open(QIODevice::ReadWrite);

    int bufferSize = buffer->write(requestString.toUtf8());
    buffer->seek(0);

    QByteArray contentLength;
    contentLength.append(QString::number(bufferSize).toStdString());

    request.setRawHeader("Content-Length", contentLength);

    //No custom request, just a put
    _reply = _manager->put(request, buffer);

    // When request ends check the status (200 OK or not) and then handle the Reply
    //connect(_reply, SIGNAL(finished()), this, SLOT(checkResponseStatus())); //TODO: This slot is not OK since it handle some signals for calendar Add!!
    connect(_reply, SIGNAL(finished()), this, SLOT(handleAddingVEventFinished()));
    // If authentication is required, provide credentials
    connect(_manager, &QNetworkAccessManager::authenticationRequired, this, &Calendar::handleAuthentication);
}

void Calendar::handleAddingVEventFinished(){
    _statusCode = _reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if (_statusCode >= 200 && _statusCode < 300) {
        qDebug() << "Evento aggiunto correttamente";
        emit eventAddFinished();

    } else {
        qDebug() << "Evento non aggiunto. Errore: " << _statusCode;

        emit eventRetrieveError();
    }
}
