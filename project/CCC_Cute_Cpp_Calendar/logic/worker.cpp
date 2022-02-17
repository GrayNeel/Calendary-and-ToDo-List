#include "worker.h"

#define REFRESH_TIMER 90    //seconds

Worker::Worker(QObject *parent) : QObject(parent)
{

}

void Worker::update()
{
    qDebug()<< "Sono nel thread";
    while(true){
        std::this_thread::sleep_for(std::chrono::seconds(REFRESH_TIMER));
        qDebug()<< "Sveglio";
        emit timeoutRefresh();
    }
}
