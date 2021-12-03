#include "eventdialog.h"
#include "ui_eventdialog.h"

EventDialog::EventDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EventDialog)
{
    ui->setupUi(this);
    ui->startingHoursLine->setText(QString("00"));
    ui->startingMinutesLine->setText(QString("00"));
    ui->endingHoursLine->setText(QString("00"));
    ui->endingMinutesLine->setText(QString("00"));
    ui->startingHoursLine->setValidator(new QIntValidator(0, 23, ui->startingHoursLine));
    ui->startingMinutesLine->setValidator(new QIntValidator(0, 59, ui->startingMinutesLine));
    ui->endingHoursLine->setValidator(new QIntValidator(0, 23, ui->endingHoursLine));
    ui->endingMinutesLine->setValidator(new QIntValidator(0, 59, ui->endingMinutesLine));
}

EventDialog::~EventDialog()
{
    delete ui;
}

void EventDialog::on_confirmButton_clicked()
{
    QString summary = QString(ui->summaryLine->text());

    if(summary.length()<1) {
        ui->errorMessageLabel->setText(QString("L'evento deve avere un titolo."));
        return;
    }

    QString location = QString(ui->locationLine->text()); // May be optional?

    QString description = QString(ui->descriptionText->toPlainText()); // May be optional?

    QDate startingDate = ui->calendarWidgetStart->selectedDate();
    QDate endingDate = ui->calendarWidgetEnd->selectedDate();

    /**
     * Validation of Date and Time received from GUI
     **/
    if(startingDate.isNull() || endingDate.isNull()) {
        ui->errorMessageLabel->setText(QString("Devi selezionare correttamente una data di inizio e di fine evento."));
        return;
    }

    QDateTime startDateTime;
    QDateTime endDateTime;

    startDateTime.setDate(startingDate);
    endDateTime.setDate(endingDate);

    if(ui->startingHoursLine->text().isNull() || ui->startingMinutesLine->text().isNull() || ui->endingHoursLine->text().isNull() || ui->endingMinutesLine->text().isNull() ||
            ui->startingHoursLine->text().length() == 0 || ui->startingMinutesLine->text().length() == 0 || ui->endingHoursLine->text().length() == 0 || ui->endingMinutesLine->text().length() == 0) {
        ui->errorMessageLabel->setText(QString("I campi orario non possono essere vuoti."));
        return;
    }
    int startingHours = ui->startingHoursLine->text().toInt();

    if(startingHours < 0 || startingHours > 24) {
        ui->errorMessageLabel->setText(QString("Ora di inizio non corretta."));
        return;
    }

    int startingMinutes = ui->startingMinutesLine->text().toInt();

    if(startingMinutes < 0 || startingMinutes > 59) {
        ui->errorMessageLabel->setText(QString("Minuti di inizio non corretti."));
        return;
    }

    startDateTime.setTime(QTime(startingHours,startingMinutes));

    int endingHours = ui->endingHoursLine->text().toInt();

    if(endingHours < 0 || endingHours> 24) {
        ui->errorMessageLabel->setText(QString("Ora di fine non corretta."));
        return;
    }

    int endingMinutes = ui->endingMinutesLine->text().toInt();

    if(endingMinutes < 0 || endingMinutes > 59) {
        ui->errorMessageLabel->setText(QString("Minuti di fine non corretti."));
        return;
    }

    endDateTime.setTime(QTime(endingHours,endingMinutes));

    if(startDateTime>=endDateTime) {
         ui->errorMessageLabel->setText(QString("La data di fine deve essere successiva a quella di inizio"));
         return;
    }

    /**
     * End of Date and Time validation
     **/

    // Not implemented yet
    // QString _rrule;

    ui->errorMessageLabel->setText("");
    emit eventAddEvent(summary, location, description, startDateTime, endDateTime);

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
