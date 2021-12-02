#ifndef EVENT_H
#define EVENT_H

#include <QObject>
#include <QDateTime>

class Event : public QObject
{
    Q_OBJECT
public:
    explicit Event(QObject *parent = nullptr);
    Event(const QString &uid, const QString &filename, const QString &summary, const QString &location, const QString &description, const QString &rrule, const QString &exdate, const QDateTime &startDateTime, const QDateTime &endDateTime);

    void setDescription(const QString &newDescription);

    void setUid(const QString &newUid);
    void setFilename(const QString &newFilename);
    void setSummary(const QString &newSummary);
    void setLocation(const QString &newLocation);
    void setRrule(const QString &newRrule);
    void setExdate(const QString &newExdate);
    void setStartDateTime(const QDateTime &newStartDateTime);
    void setEndDateTime(const QDateTime &newEndDateTime);

    const QString &uid() const;

    const QString &filename() const;

    const QString &summary() const;
    const QString &location() const;
    const QString &description() const;
    const QString &rrule() const;
    const QString &exdate() const;
    const QDateTime &startDateTime() const;
    const QDateTime &endDateTime() const;

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
