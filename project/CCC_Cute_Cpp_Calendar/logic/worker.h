#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QDebug>
#include <thread>
#include <chrono>
#include <mutex>

class Worker : public QObject
{
    Q_OBJECT
public:
    explicit Worker(QObject *parent = nullptr);
public slots:
    void update();
signals:
    void timeoutRefresh();
};

#endif // WORKER_H
