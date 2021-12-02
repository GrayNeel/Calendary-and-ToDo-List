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

    //Se usiamo questi widget dobbiamo inserire dei check sulle ore o quantomeno usare un widget di tipo TimeEdit
    //Oppure si sceglie il widget che genera automaticamente oggetti QDateTime
//    QDate startDate = ui->calendarWidget->selectedDate();
//    QDate endDate= ui->calendarWidget_2->selectedDate();
//    //h must be in the range 0 to 23, m and s must be in the range 0 to 59, and ms must be in the range 0 to 999.
//    //Insert check on the field
//    QTime startTime= QTime(ui->startingHoursLine->text().toInt(), ui->startingMinutesLine->text().toInt());
//    QTime endTime= QTime(ui->endingHoursLine->text().toInt(), ui->endingMinutesLine->text().toInt());
//    QDateTime startDateTime = QDateTime(startDate, startTime);
//    QDateTime endDateTime = QDateTime(endDate, endTime);

    //Inserire check sulle date (seconda successiva alla prima)
    QDateTime startDateTime = ui->dateTimeEditStarting->dateTime();
    QDateTime endDateTime = ui->dateTimeEditEnding->dateTime();

    if(startDateTime>=endDateTime)
         ui->errorMessageLabel->setText(QString("La data di fine deve essere successiva a quella di inizio"));
    else {
         emit eventAddEvent(summary, location, description, startDateTime, endDateTime);
    }

}

void EventDialog::setCal(Calendar *newCal)
{
    cal = newCal;
}

//TODO: Implement this
void EventDialog::handleEventResponse(QString errorMessage) {

}

void EventDialog::setCalName(QString calName) {
    ui->calendarNameEdit->setText(calName);
}
