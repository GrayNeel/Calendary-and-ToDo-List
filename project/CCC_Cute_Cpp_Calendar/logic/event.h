#ifndef EVENT_H
#define EVENT_H

#include <QObject>
#include <QDateTime>

class Event : public QObject
{
    Q_OBJECT
public:
    explicit Event(QObject *parent = nullptr);

signals:

public slots:

private slots:

private:
    QString _uid;
    QString _filename;
    QString _summary;
    QString _location;
    QString _description;
    QString _rrule;
    QString _exdate;
    QDateTime _startDateTime;
    QDateTime _endDateTime;
};

#endif // EVENT_H
