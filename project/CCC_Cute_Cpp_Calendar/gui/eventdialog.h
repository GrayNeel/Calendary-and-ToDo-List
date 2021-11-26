#ifndef EVENTDIALOG_H
#define EVENTDIALOG_H

#include <QDialog>

namespace Ui {
class EventDialog;
}

class EventDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EventDialog(QWidget *parent = nullptr);
    ~EventDialog();

signals:
    void eventAddEvent(QString summary, QString location, QString description, QDateTime startDateTime, QDateTime endDateTime);

public slots:
    void handleEventResponse(QString errorMessage);

private slots:
    void on_confirmButton_clicked();

private:
    Ui::EventDialog *ui;
};

#endif // EVENTDIALOG_H
