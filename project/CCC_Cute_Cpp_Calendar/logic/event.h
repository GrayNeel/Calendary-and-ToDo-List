#ifndef EVENT_H
#define EVENT_H

#include <QObject>
#include <QDateTime>

class Event : public QObject
{
    Q_OBJECT
public:
    explicit Event(QObject *parent = nullptr);

    void setDescription(const QString &newDescription);

    void setUid(const QString &newUid);
    void setFilename(const QString &newFilename);
    void setSummary(const QString &newSummary);
    void setLocation(const QString &newLocation);
    void setRrule(const QString &newRrule);
    void setExdate(const QString &newExdate);
    void setStartDateTime(const QDateTime &newStartDateTime);
    void setEndDateTime(const QDateTime &newEndDateTime);

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
