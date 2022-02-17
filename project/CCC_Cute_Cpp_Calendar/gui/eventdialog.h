#ifndef EVENTDIALOG_H
#define EVENTDIALOG_H

#include <QDialog>
#include <QDateTime>
#include <QDate>
#include <QValidator>
#include "logic/calendar.h"

namespace Ui {
class EventDialog;
}

class EventDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EventDialog(QWidget *parent = nullptr);
    ~EventDialog();

    void setCal(Calendar *newCal);
    void setCalName(QString calname);
    void setFields();
    void disableFields(bool status);

    Calendar *getCal() const;

    Event *getEvent() const;
    void setEvent(Event *newEv);

    void setUpdating(bool value);

signals:
    void eventAddEvent(QString summary, QString location, QString description, QDateTime startDateTime, QDateTime endDateTime);
    void eventModifyEvent(QString oldUid, QString summary, QString location, QString description, QDateTime startDateTime, QDateTime endDateTime);
    void closeEventDialog();

public slots:

private slots:
    void on_confirmButton_clicked();

    void on_abortButton_clicked();

    void on_EventDialog_rejected();

private:
    Ui::EventDialog *ui;
    Calendar* cal;

    Event* event;
    bool updating;


    void handleEventResponse(QString errorMessage);
};

#endif // EVENTDIALOG_H
