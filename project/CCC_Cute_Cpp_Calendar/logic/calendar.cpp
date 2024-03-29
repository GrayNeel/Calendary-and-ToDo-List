#include "calendar.h"

Calendar::Calendar(QObject *parent) : QObject(parent)
{
    // Create a QNetworkAccessManager that will be unique for this specific calendar
    _manager = new QNetworkAccessManager(this);
    _parent = parent;
    connect(parent,SIGNAL(requestSyncToken()),this,SLOT(APIRequestSyncToken()));
}

Calendar::~Calendar(void) {
    delete _manager;
//    disconnect(_parent,SIGNAL(requestSyncToken()),this,SLOT(APIRequestSyncToken()));
//    disconnect(this,SIGNAL(calendarRetrieveError()),_parent,SLOT(handleAddCalendarError()));
//    disconnect(this,SIGNAL(calendarAdded(QString)), _parent, SLOT(handleAddCalendarFinished(QString)));
//    disconnect(this, SIGNAL(refreshEventVisualization()), _parent->parent(), SLOT(handleUpdateMainWindowWidgets()));
//    disconnect(this,SIGNAL(refreshLocalCalendarData(Calendar*)),_parent,SLOT(handleRefreshCalendar(Calendar*)));
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

void Calendar::eraseEventsTodos() {
    for(Event* ev : _eventsList) {
        ev->blockSignals(true);
        delete ev;
    }

    for(Todo* td : _todosList) {
        td->blockSignals(true);
        delete td;
    }

    _eventsList.clear();
    _todosList.clear();
}

/*************************** API Requests to CalDAV Server ****************************/

/**
 * @brief This API Requests a Sync Token to the server
 */
void Calendar::APIRequestSyncToken(void) {
    disconnect(_parent,SIGNAL(requestSyncToken()),this,SLOT(APIRequestSyncToken()));
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
        //qDebug() << "Bad URL";
        emit calendarRetrieveError(QString("Wrong URL. Try again!"), _url);
    }

    if (_statusCode >= 200 && _statusCode < 300) {
        //qDebug() << "Good Response: " << _statusCode;

    } else if (_statusCode > 0){
        //qDebug() << "Bad Response: " << _statusCode;
        emit calendarRetrieveError(QString("Wrong credentials. Try again!"), _url);
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
        parseResponse();
        emit calendarAdded(_url);
    }
}

void Calendar::parseResponse() {
    QDomDocument body;
    body.setContent(_reply);

    //We have to refresh local list of event
    _eventsList.clear();

    //Response of the repor_t could be a multistatus, with multiple response child
    QDomNodeList list_response = body.elementsByTagName("d:response");
    for (int i = 0; i < list_response.size(); i++)
    {
      //structure of a repsonse
      //Response -> href -> propstat ->
      //                                prop -> getetag
      //                                     -> cal:calendar-data
      //                                status -> 200 OK
      QDomNode thisResponse = list_response.item(i);

      QString uri = "";
      QString eTag = "";

      QDomElement href = thisResponse.firstChildElement("d:href");
      if (!href.isNull())
        uri = href.text();



      QDomNode thisPropStat = thisResponse.firstChildElement("d:propstat");
      if (!thisPropStat.isNull())
      {
        QDomNode thisProp = thisPropStat.firstChildElement("d:prop");
        if (!thisProp.isNull())
        {
          QDomElement bodyEtag = thisProp.firstChildElement("d:getetag");
          if (!bodyEtag.isNull())
            eTag = bodyEtag.text();

          QDomElement bodyCalendarData = thisProp.firstChildElement("cal:calendar-data");
          if (!bodyCalendarData.isNull())
          {
            //It's equal for todo and event
            parseCalendarData(bodyCalendarData.text(), uri, eTag);
          }
        }
      }
    }
}

void Calendar::parseCalendarData(QString entity, QString uri, QString eTag){
    //Create a map with all possible element in a VTODO or VEVENT (SUMMARY, RRULE, EXDATA, LOCATION...)
    QMap<QString, QString> map;
    QList<QString> lines = entity.split("\n");

    //manage event
    if(entity.contains("BEGIN:VEVENT")){
    //0//BEGIN:VCALENDAR
    //1//VERSION:2.0
    //2//PRODID:-//Sabre//Sabre VObject 4.2.2//EN
    //3//BEGIN:VEVENT
    //4//UID:uriDiProva
    //5//DTSTAMP:20211203T164559Z
    //6//DTSTART:20211224T042000
    //7//DTEND:20211231T000000
    //8//SUMMARY:Prova
    //9//LOCATION:Luogo di prova
    //10//DESCRIPTION:descrizione di prova
    //11//END:VEVENT
    //12//END:VCALENDAR
        //Every VCalendar object could contain only a "real" entity (VEVENT ir VTODO)
        //Strip off useless lines
        int start = lines.indexOf("BEGIN:VEVENT");
        int numberOfUsefulLines = lines.indexOf("END:VEVENT") - start +1;
        QList<QString> event = lines.sliced(start, numberOfUsefulLines);
        for(int i=0; i<event.length(); i++){
            QVector<QString> line = event[i].split(":");
            map.insert(line[0], line[1]);
        }
        Event *e = new Event();
        //We can check for a specific property or try to get value and assign it by default
        //if (map.contains("SUMMARY"))
        //  summary = map.value("SUMMARY");
        QString uid = map.value("UID", "");
        //verificare se il formato è giusto
        //20211203T164559Z
        QDateTime dtStamp = QDateTime::fromString(map.value("DTSTAMP"), "yyyyMMddTHHmmssZ");
        QDateTime dtStart = QDateTime::fromString(map.value("DTSTART"), "yyyyMMddTHHmmss");
        QDateTime dtEnd = QDateTime::fromString(map.value("DTEND"), "yyyyMMddTHHmmss");
        QString summary = map.value("SUMMARY", "");
        QString location = map.value("LOCATION", "");
        QString description = map.value("DESCRIPTION", "");
        e->setUid(uid);
        e->setStartDateTime(dtStart);
        e->setEndDateTime(dtEnd);
        e->setSummary(summary);
        e->setLocation(location);
        e->setDescription(description);

        //e->setFilename(uri);
        //Sbagliato, l'uri è solo l'ultima parte
        //QString filename = uri.replace(_url, "");
        QList<QString> resources = uri.split("/");
        QString filename = resources.last();
        qDebug() << filename;
        e->setFilename(filename);
        e->setEtag(eTag);

        e->setColour(_colour);
        _eventsList.append(e);

        connect(e, &Event::removeEvent, this, &Calendar::deleteEvent);
        //ADD what you want
        //dt are QString and not QDateTime... find a way to construct it from the string

    }//manage todo
    else if (entity.contains("BEGIN:VTODO")){
                    //1VERSION:2.0
                    //2CALSCALE:GREGORIAN
                    //3BEGIN:VTODO
                    //4UID:132456762153245
                    //5SUMMARY:Do the dishes
                    //6DUE:20121028T115600Z
                    //7END:VTODO
                    //8END:VCALENDAR
        //Every VCalendar object could contain only a "real" entity (VEVENT ir VTODO)
        //Strip off useless lines
        int start = lines.indexOf("BEGIN:VTODO");
        int numberOfUsefulLines = lines.indexOf("END:VTODO") - start +1;
        QList<QString> todo = lines.sliced(start, numberOfUsefulLines);
        for(int i=0; i<todo.length(); i++){
            QVector<QString> line = todo[i].split(":");
            map.insert(line[0], line[1]);
        }
        Todo *t = new Todo();
        //We can check for a specific property or try to get value and assign it by default
        //if (map.contains("SUMMARY"))
        //  summary = map.value("SUMMARY");
        QString uid = map.value("UID", "");
        //verificare se il formato è giusto
        //20211203T164559Z
        QDateTime dtDue = QDateTime::fromString(map.value("DUE"), "yyyyMMddTHHmmssZ");
        QString summary = map.value("SUMMARY", "");
        t->setUid(uid);
        t->setDueDateTime(dtDue);
        t->setSummary(summary);

        //e->setFilename(uri);
        //Sbagliato, l'uri è solo l'ultima parte
        //QString filename = uri.replace(_url, "");
        QList<QString> resources = uri.split("/");
        QString filename = resources.last();
        qDebug() << filename;
        t->setFilename(filename);
        t->setEtag(eTag);

        t->setColour(_colour);
        _todosList.append(t);

        connect(t, &Todo::removeTodo, this, &Calendar::deleteTodo);
        //ADD what you want
        //dt are QString and not QDateTime... find a way to construct it from the string
    }
}

void Calendar::handleRemoveCalendar(){
    emit removeCalendar(this);
}

void Calendar::handleAddNewEventPopUp() {
    emit showEventDialog(this);
}

void Calendar::handleAddNewTodoPopUp() {
    emit showTodoDialog(this);
}

void Calendar::handleAddEvent(QString summary, QString location, QString description, QDateTime startDateTime, QDateTime endDateTime) {
    //if uid=-1 I have a new event. Constructor will choose a new one based on timestamp
    //if filename is empty constructor create it basing on uid
    //update API is based on a specific uid
    Event* newEvent = new Event(QString("-1"), QString(""), summary, location, description, QString(""), QString(""), startDateTime, endDateTime, _colour);
    _eventsList.append(newEvent);
    connect(newEvent, &Event::removeEvent, this, &Calendar::deleteEvent);
    APIAddEvent(newEvent);
}

void Calendar::handleModifyEvent(QString oldUid, QString summary, QString location, QString description, QDateTime startDateTime, QDateTime endDateTime) {
    qDebug() << "Voglio modificare evento";
    Event* oldEvent;
    for(Event* ev : _eventsList) {
        if(ev->uid() == oldUid) {
            oldEvent = ev;
            break;
        }
    }
    //Se l'update non andrà a buon fine cancellerò la risorsa e farò l'update generale
    Event* newEvent = new Event(QString(oldUid), QString(""), summary, location, description, QString(""), QString(""), startDateTime, endDateTime, _colour);
    QString oldEtag = oldEvent->etag();
    _eventsList.removeOne(oldEvent);
    _eventsList.append(newEvent);
    APIUpdateEvent(newEvent, oldEtag);
}
/**
 *
 * @brief This API updates an existing event (a VCalendar Object) in a specific calendar
 */
void Calendar::APIUpdateEvent(Event* event, QString etag) {
    // https://datatracker.ietf.org/doc/html/rfc4791#section-5.3.2
    QNetworkRequest request;

    // Building the header of a PUT request to push a VCalendar Object for ONE VEVENT

    request.setUrl(QUrl(_url + event->filename()));
    request.setAttribute(QNetworkRequest::Http2AllowedAttribute, false); // Fallback to HTTP 1.1
    //"The "If-Match: etag" request header ensures that the client will not inadvertently overwrite a resource updated from someone else
    //This property allow to work with asynchronously shared access to the resource.
    request.setRawHeader("If-Match", QByteArray(etag.toLatin1()));
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
    //connect(_reply, SIGNAL(finished()), this, SLOT(checkResponseStatus())); //This slot is not OK since it handle some signals for calendar Add!
    connect(_reply, SIGNAL(finished()), this, SLOT(handleAddingVEventFinished()));  //so we use this
    // If authentication is required, provide credentials
    connect(_manager, &QNetworkAccessManager::authenticationRequired, this, &Calendar::handleAuthentication);
}

void Calendar::getForLastVEventResource(QUrl resourceUrl){
    QNetworkRequest request;

    // Building the header of a GET request to just retrieve the etag for the last event added

    request.setUrl(resourceUrl);
    request.setAttribute(QNetworkRequest::Http2AllowedAttribute, false); // Fallback to HTTP 1.1
    request.setRawHeader("Depth", "0");
    request.setRawHeader("Prefer", "return-minimal");
    request.setRawHeader("Content-Type", "application/xml; charset=utf-8");

    _reply = _manager->get(request);
    // When request ends check the status (200 OK or not) and then handle the Reply
    connect(_reply, SIGNAL(finished()), this, SLOT(handleGetVEventFinished()));
    // If authentication is required, provide credentials
    connect(_manager, &QNetworkAccessManager::authenticationRequired, this, &Calendar::handleAuthentication);

}

void Calendar::getForLastTodoResource(QUrl resourceUrl){
    QNetworkRequest request;

    // Building the header of a GET request to just retrieve the etag for the last event added

    request.setUrl(resourceUrl);
    request.setAttribute(QNetworkRequest::Http2AllowedAttribute, false); // Fallback to HTTP 1.1
    request.setRawHeader("Depth", "0");
    request.setRawHeader("Prefer", "return-minimal");
    request.setRawHeader("Content-Type", "application/xml; charset=utf-8");

    _reply = _manager->get(request);
    // When request ends check the status (200 OK or not) and then handle the Reply
    connect(_reply, SIGNAL(finished()), this, SLOT(handleGetTodoFinished()));
    // If authentication is required, provide credentials
    connect(_manager, &QNetworkAccessManager::authenticationRequired, this, &Calendar::handleAuthentication);

}

void Calendar::handleGetVEventFinished(){
    _statusCode = _reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if (_statusCode >= 200 && _statusCode < 300) {
        qDebug() << "Get eseguita correttamente: "<< _statusCode;
        if(_reply->hasRawHeader(QByteArray("ETag"))){
            //Save the etag in the event with the name of the url resource/or the last one appended
            QString etag = _reply->header(QNetworkRequest::ETagHeader).toString();
            _eventsList.last()->setEtag(etag);
            qDebug()<< "Etag dell'ultimo oggetto (recuperato dalla get):" << etag;
            qDebug() << "Ultimo evento aggiunto: " << _eventsList.last()->filename();
            qDebug() << "Etag aggiornato: " << _eventsList.last()->etag();
        }
        else
            qDebug()<< "Errore: nessun header Etag trovato nella risposta";
        //emit eventAddFinished();
    //si potrebbe anche rifare richiesta per l'etag
    } else {
        qDebug() << "Get errata: " << _statusCode;
        //_eventsList.removeLast();
        //emit eventRetrieveError();
    }
}

void Calendar::handleGetTodoFinished(){
    _statusCode = _reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if (_statusCode >= 200 && _statusCode < 300) {
        qDebug() << "Get eseguita correttamente: "<< _statusCode;
        if(_reply->hasRawHeader(QByteArray("ETag"))){
            //Save the etag in the event with the name of the url resource/or the last one appended
            QString etag = _reply->header(QNetworkRequest::ETagHeader).toString();
            _todosList.last()->setEtag(etag);
            qDebug()<< "Etag dell'ultimo oggetto (recuperato dalla get):" << etag;
            qDebug() << "Ultimo evento aggiunto: " << _todosList.last()->filename();
            qDebug() << "Etag aggiornato: " << _todosList.last()->etag();
        }
        else
            qDebug()<< "Errore: nessun header Etag trovato nella risposta";
        //emit todoAddFinished();
    //si potrebbe anche rifare richiesta per l'etag
    } else {
        qDebug() << "Get errata: " << _statusCode;
        //_todosList.removeLast();
        //emit todoRetrieveError();
    }
}

void Calendar::handleAddingVEventFinished(){
    _statusCode = _reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    //QUrl resourceUri = _reply->url();
    QUrl resourceUrl = _reply->request().url();

    if (_statusCode >= 200 && _statusCode < 300) {
        qDebug() << "Evento aggiunto correttamente";
        //GET alla stessa risorsa per prendere l'etag e salvarlo dentro l'evento specifico (con il nome della risorsa)
        getForLastVEventResource(resourceUrl);
        emit eventAddFinished();
        //I can connect two slot to the same signal... but maybe I want to create completely different API for update
        emit eventModifyFinished();

    } else if (_statusCode == 412) {  //412 Precondition Failed
      //Refreshare calendario in caso di errore (ESEMPIO UPDATE SIMULTANEO DELLA RISORSA)
        emit eventRetrieveError();
        emit eventModifyRetrieveError();
        emit refreshLocalCalendarData(this);
    }
    else {
        qDebug() << "Evento non aggiunto/aggiornato. Errore: " << _statusCode;
        _eventsList.removeLast();
        emit eventRetrieveError();
        emit eventModifyRetrieveError();
    }
}


void Calendar::deleteEvent(Event* event){
    QNetworkRequest request;

    // Building the header of a DELETE request to delete a VCalendar Object of ONE VEVENT

    request.setUrl(QUrl(_url + event->filename()));
    request.setAttribute(QNetworkRequest::Http2AllowedAttribute, false); // Fallback to HTTP 1.1
    //"The "If-None-Match: *" request header ensures that the client will not inadvertently overwrite an existing resource
    //if the last path segment turned out to already be used"
    request.setRawHeader("If-Match", QByteArray(event->etag().toLatin1()));
    request.setRawHeader("Content-Type", "text/calendar; charset=utf-8");

    //No custom request, just a put
    _reply = _manager->sendCustomRequest(request, QByteArray("DELETE"));

    // When request ends check the status (200 OK or not) and then handle the Reply
    //connect(_reply, SIGNAL(finished()), this, SLOT(checkResponseStatus())); //This slot is not OK since it handle some signals for calendar Add!
    connect(_reply, SIGNAL(finished()), this, SLOT(handleDeletingVEventFinished()));    //so we use this
    // If authentication is required, provide credentials
    connect(_manager, &QNetworkAccessManager::authenticationRequired, this, &Calendar::handleAuthentication);

}

void Calendar::handleDeletingVEventFinished(){
    _statusCode = _reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    //QUrl resourceUri = _reply->url();
    QUrl resourceUrl = _reply->request().url();
    QList<QString> resources = resourceUrl.toString().split("/");
    QString filename = resources.last();


    if (_statusCode >= 200 && _statusCode < 300) {
        qDebug() << "Evento eliminato correttamente";
        for(Event* ev: _eventsList){
            if(ev->filename().compare(filename) == 0){
                _eventsList.removeOne(ev);
                break;
            }
        }
        emit refreshEventVisualization();

    } else {
        if(_statusCode == 412){
            qDebug() << "Evento non aggiunto. Errore: 412. \"Precondition failed\" perchè l'etag è sbagliato" << _statusCode;
            emit refreshLocalCalendarData(this);
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setText("L'evento è già stato rimosso da qualcun altro");
            msgBox.exec();
        } else {
            qDebug() << "Evento non rimosso. Errore: " << _statusCode;
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setText("L'evento NON è stato rimosso");
            msgBox.exec();
        }
    }
}

void Calendar::handleAddTodo(QString summary, QDateTime dueDateTime) {
    //if uid=-1 I have a new event. Constructor will choose a new one based on timestamp
    //if filename is empty constructor create it basing on uid
    //update API is based on a specific uid

    Todo* newTodo = new Todo(QString("-1"), QString(""), summary, dueDateTime, _colour);
    _todosList.append(newTodo);
    connect(newTodo, &Todo::removeTodo, this, &Calendar::deleteTodo);
    APIAddTodo(newTodo);
}

void Calendar::handleModifyTodo(QString oldUid, QString summary, QDateTime dueDateTime) {
    qDebug() << "Voglio modificare un todo";
    Todo* oldTodo;
    for(Todo* td : _todosList) {
        if(td->uid() == oldUid) {
            oldTodo = td;
            break;
        }
    }
    //Se l'update non andrà a buon fine cancellerò la risorsa e farò l'update generale
    Todo* newTodo = new Todo(QString(oldUid), QString(""), summary, dueDateTime, _colour);
    QString oldEtag = oldTodo->etag();
    _todosList.removeOne(oldTodo);
    _todosList.append(newTodo);
    APIUpdateTodo(newTodo, oldEtag);
}
/**
 *
 * @brief This API update a todo (a VCalendar Object) in a specific calendar
 */
void Calendar::APIUpdateTodo(Todo* todo, QString etag) {
    // https://datatracker.ietf.org/doc/html/rfc4791#section-5.3.2
    QNetworkRequest request;

    // Building the header of a PUT request to push a VCalendar Object for ONE VEVENT

    request.setUrl(QUrl(_url + todo->filename()));
    request.setAttribute(QNetworkRequest::Http2AllowedAttribute, false); // Fallback to HTTP 1.1
    //"The "If-Match: etag" request header ensures that the client will not inadvertently overwrite a resource updated from someone else
    //This property allow to work with asynchronously shared access to the resource.
    request.setRawHeader("If-Match", QByteArray(etag.toLatin1()));
    request.setRawHeader("Content-Type", "text/calendar; charset=utf-8");

    // Building the Body
    QString requestString = "BEGIN:VCALENDAR\r\n"
                            "VERSION:2.0\r\n"
                            "BEGIN:VTODO\r\n"
                            "UID:" + todo->uid() + "\r\n"
                            "SUMMARY:" + todo->summary() + "\r\n"
                            "DUE:" + todo->dueDateTime().toString("yyyyMMddTHHmmss") + "Z\r\n"
                            "END:VTODO\r\nEND:VCALENDAR";

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
    //connect(_reply, SIGNAL(finished()), this, SLOT(checkResponseStatus())); //This slot is not OK since it handle some signals for calendar Add!
    connect(_reply, SIGNAL(finished()), this, SLOT(handleAddingVTodoFinished()));   //so we use this
    // If authentication is required, provide credentials
    connect(_manager, &QNetworkAccessManager::authenticationRequired, this, &Calendar::handleAuthentication);
}

/**
 *
 * @brief This API push a new event in a VCalendar Object from a specific calendar
 */
void Calendar::APIAddTodo(Todo* todo) {
    // https://datatracker.ietf.org/doc/html/rfc4791#section-5.3.2
    QNetworkRequest request;

    // Building the header of a PUT request to push a VCalendar Object for ONE VEVENT

    request.setUrl(QUrl(_url + todo->filename()));
    request.setAttribute(QNetworkRequest::Http2AllowedAttribute, false); // Fallback to HTTP 1.1
    //"The "If-None-Match: *" request header ensures that the client will not inadvertently overwrite an existing resource
    //if the last path segment turned out to already be used"
    request.setRawHeader("If-None-Match", "*");
    request.setRawHeader("Content-Type", "text/calendar; charset=utf-8");

    // Building the Body
    QString requestString = "BEGIN:VCALENDAR\r\n"
                            "VERSION:2.0\r\n"
                            "BEGIN:VTODO\r\n"
                            "UID:" + todo->uid() + "\r\n"
                            "SUMMARY:" + todo->summary() + "\r\n"
                            "DUE:" + todo->dueDateTime().toString("yyyyMMddTHHmmss") + "Z\r\n"
                            "END:VTODO\r\nEND:VCALENDAR";

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
    //connect(_reply, SIGNAL(finished()), this, SLOT(checkResponseStatus())); //This slot is not OK since it handle some signals for calendar Add!
    connect(_reply, SIGNAL(finished()), this, SLOT(handleAddingVTodoFinished()));   //so we use this
    // If authentication is required, provide credentials
    connect(_manager, &QNetworkAccessManager::authenticationRequired, this, &Calendar::handleAuthentication);
}

void Calendar::handleAddingVTodoFinished(){
    _statusCode = _reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    //QUrl resourceUri = _reply->url();
    QUrl resourceUrl = _reply->request().url();

    if (_statusCode >= 200 && _statusCode < 300) {
        qDebug() << "Todo aggiunto correttamente";
        //GET alla stessa risorsa per prendere l'etag e salvarlo dentro l'evento specifico (con il nome della risorsa)
        getForLastTodoResource(resourceUrl);
        emit todoAddFinished();
        emit todoModifyFinished();
    }        //Refreshare calendario in caso di errore (ESEMPIO UPDATE SIMULTANEO DELLA RISORSA)
    else if(_statusCode == 412) {//412 Precondition Failed
        emit todoRetrieveError();
        emit todoModifyRetrieveError();
        emit refreshLocalCalendarData(this);
    } else {

        qDebug() << "Todo non aggiunto. Errore: " << _statusCode;
        _todosList.removeLast();
        emit todoRetrieveError();
        emit todoModifyRetrieveError();
    }
}

const QList<Todo *> &Calendar::todosList() const
{
    return _todosList;
}

void Calendar::deleteTodo(Todo* todo){
    QNetworkRequest request;

    // Building the header of a DELETE request to delete a VCalendar Object of ONE VTODO

    request.setUrl(QUrl(_url + todo->filename()));
    request.setAttribute(QNetworkRequest::Http2AllowedAttribute, false); // Fallback to HTTP 1.1
    //"The "If-None-Match: *" request header ensures that the client will not inadvertently overwrite an existing resource
    //if the last path segment turned out to already be used"
    request.setRawHeader("If-Match", QByteArray(todo->etag().toLatin1()));
    request.setRawHeader("Content-Type", "text/calendar; charset=utf-8");

    //No custom request, just a put
    _reply = _manager->sendCustomRequest(request, QByteArray("DELETE"));

    // When request ends check the status (200 OK or not) and then handle the Reply
    //connect(_reply, SIGNAL(finished()), this, SLOT(checkResponseStatus())); //This slot is not OK since it handle some signals for calendar Add!
    connect(_reply, SIGNAL(finished()), this, SLOT(handleDeletingVTodoFinished())); //so we use this
    // If authentication is required, provide credentials
    connect(_manager, &QNetworkAccessManager::authenticationRequired, this, &Calendar::handleAuthentication);

}

void Calendar::handleDeletingVTodoFinished(){
    _statusCode = _reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    //QUrl resourceUri = _reply->url();
    QUrl resourceUrl = _reply->request().url();
    QList<QString> resources = resourceUrl.toString().split("/");
    QString filename = resources.last();


    if (_statusCode >= 200 && _statusCode < 300) {
        qDebug() << "Todo eliminato correttamente";
        for(Todo* td: _todosList){
            if(td->filename().compare(filename) == 0){
                _todosList.removeOne(td);
                break;
            }
        }
        emit refreshEventVisualization();

    } else {
        if(_statusCode == 412){
            qDebug() << "Todo non aggiunto. Errore: 412. \"Precondition failed\" perchè l'etag è sbagliato" << _statusCode;
            emit refreshLocalCalendarData(this);
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setText("Il Todo è stato già rimosso da qualcun altro");
            msgBox.exec();
        } else {

            qDebug() << "Todo non rimosso. Errore: " << _statusCode;
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setText("Il Todo NON è stato rimosso");
            msgBox.exec();
        }
    }
}

const QString &Calendar::colour() const
{
    return _colour;
}

void Calendar::setColour(const QString &newColour)
{
    _colour = newColour;
}

const QList<Event *> &Calendar::eventsList() const
{
    return _eventsList;
}
