#include "eventdialog.h"
#include "ui_eventdialog.h"

EventDialog::EventDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EventDialog)
{
    ui->setupUi(this);
}

EventDialog::~EventDialog()
{
    delete ui;
}

void EventDialog::on_confirmButton_clicked()
{
    QString summary = QString(ui->summaryLine->text());
    QString location = QString(ui->locationLine->text());
    QString description = QString(ui->descriptionText->toPlainText());
    // Not implemented yet
    // QString _rrule;

    QDateTime startDateTime;
    QDateTime endDateTime;

    emit eventAddEvent(summary, location, description, startDateTime, endDateTime);
}

//TODO: Implement this
void EventDialog::handleEventResponse(QString errorMessage) {

}
