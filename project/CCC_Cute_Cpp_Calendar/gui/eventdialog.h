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

    Calendar *getCal() const;

signals:
    void eventAddEvent(QString summary, QString location, QString description, QDateTime startDateTime, QDateTime endDateTime);

public slots:

private slots:
    void on_confirmButton_clicked();

private:
    Ui::EventDialog *ui;
    Calendar* cal;

    void handleEventResponse(QString errorMessage);
};

#endif // EVENTDIALOG_H
