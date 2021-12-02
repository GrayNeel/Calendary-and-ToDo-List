#include "event.h"

Event::Event(QObject *parent) : QObject(parent)
{

}

Event::Event(const QString &uid, const QString &filename, const QString &summary, const QString &location, const QString &description, const QString &rrule, const QString &exdate, const QDateTime &startDateTime, const QDateTime &endDateTime) : _uid(uid),
    _filename(filename),
    _summary(summary),
    _location(location),
    _description(description),
    _rrule(rrule),
    _exdate(exdate),
    _startDateTime(startDateTime),
    _endDateTime(endDateTime)
{
    if (uid.toInt()==-1)
      {
        //_uid = QDateTime::currentDateTime().toString("yyyyMMdd-HHMM-00ss") + "-0000-" + startDateTime.toString("yyyyMMddHHMM");
        _uid = "uriDiProva";
      }

      if (filename.isEmpty())
      {
        _filename = _uid + ".ics";
      }
}

void Event::setUid(const QString &newUid)
{
    _uid = newUid;
}

void Event::setFilename(const QString &newFilename)
{
    _filename = newFilename;
}

void Event::setSummary(const QString &newSummary)
{
    _summary = newSummary;
}

void Event::setLocation(const QString &newLocation)
{
    _location = newLocation;
}

void Event::setRrule(const QString &newRrule)
{
    _rrule = newRrule;
}

void Event::setExdate(const QString &newExdate)
{
    _exdate = newExdate;
}

void Event::setStartDateTime(const QDateTime &newStartDateTime)
{
    _startDateTime = newStartDateTime;
}

void Event::setEndDateTime(const QDateTime &newEndDateTime)
{
    _endDateTime = newEndDateTime;
}

const QString &Event::uid() const
{
    return _uid;
}

const QString &Event::filename() const
{
    return _filename;
}

const QString &Event::summary() const
{
    return _summary;
}

const QString &Event::location() const
{
    return _location;
}

const QString &Event::description() const
{
    return _description;
}

const QString &Event::rrule() const
{
    return _rrule;
}

const QString &Event::exdate() const
{
    return _exdate;
}

const QDateTime &Event::startDateTime() const
{
    return _startDateTime;
}

const QDateTime &Event::endDateTime() const
{
    return _endDateTime;
}

