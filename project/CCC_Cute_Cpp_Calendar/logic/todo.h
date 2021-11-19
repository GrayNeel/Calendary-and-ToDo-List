#ifndef TODO_H
#define TODO_H

#include <QObject>

class Todo : public QObject
{
    Q_OBJECT
public:
    explicit Todo(QObject *parent = nullptr);

signals:

};

#endif // TODO_H
