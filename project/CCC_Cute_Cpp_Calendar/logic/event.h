#ifndef EVENT_H
#define EVENT_H

#include <QObject>

class Event : public QObject
{
    Q_OBJECT
public:
    explicit Event(QObject *parent = nullptr);

signals:

};

#endif // EVENT_H
