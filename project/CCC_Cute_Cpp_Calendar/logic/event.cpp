#include "event.h"

Event::Event(QObject *parent) : QObject(parent)
{

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
