#include "eventdialog.h"
#include "ui_eventdialog.h"

EventDialog::EventDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EventDialog)
{
    ui->setupUi(this);

    /**
     * Setup validation of Time fields, since hours and minutes are lineEdit
     * and user might insert characters instead of numbers
     **/
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
    // Summary field & check
    QString summary = QString(ui->summaryLine->text());

    if(summary.length()<1) {
        handleEventResponse(QString("L'evento deve avere un nome."));
        return;
    }

    QString location = QString(ui->locationLine->text());
    // No validation on location since it might be optional

    QString description = QString(ui->descriptionText->toPlainText());
    // No validation on description since it might be optional

    QDate startingDate = ui->calendarWidgetStart->selectedDate();
    QDate endingDate = ui->calendarWidgetEnd->selectedDate();

    /**
     * Validation of Date and Time received from GUI
     **/
    if(startingDate.isNull() || endingDate.isNull()) {
        handleEventResponse(QString("Devi selezionare correttamente una data di inizio e di fine evento."));
        return;
    }

    QDateTime startDateTime;
    QDateTime endDateTime;

    startDateTime.setDate(startingDate);
    endDateTime.setDate(endingDate);

    if(ui->startingHoursLine->text().isNull() || ui->startingMinutesLine->text().isNull() || ui->endingHoursLine->text().isNull() || ui->endingMinutesLine->text().isNull() ||
            ui->startingHoursLine->text().length() == 0 || ui->startingMinutesLine->text().length() == 0 || ui->endingHoursLine->text().length() == 0 || ui->endingMinutesLine->text().length() == 0) {
        handleEventResponse(QString("I campi orario non possono essere vuoti."));
        return;
    }
    int startingHours = ui->startingHoursLine->text().toInt();

    if(startingHours < 0 || startingHours > 24) {
        handleEventResponse(QString("Ora di inizio non corretta."));
        return;
    }

    int startingMinutes = ui->startingMinutesLine->text().toInt();

    if(startingMinutes < 0 || startingMinutes > 59) {
        handleEventResponse(QString("Minuti di inizio non corretti."));
        return;
    }

    startDateTime.setTime(QTime(startingHours,startingMinutes));

    int endingHours = ui->endingHoursLine->text().toInt();

    if(endingHours < 0 || endingHours> 24) {
        handleEventResponse(QString("Ora di fine non corretta."));
        return;
    }

    int endingMinutes = ui->endingMinutesLine->text().toInt();

    if(endingMinutes < 0 || endingMinutes > 59) {
        handleEventResponse(QString("Minuti di fine non corretti."));
        return;
    }

    endDateTime.setTime(QTime(endingHours,endingMinutes));

    if(startDateTime>=endDateTime) {
         handleEventResponse(QString("La data di fine deve essere successiva a quella di inizio"));
         return;
    }

    /**
     * End of Date and Time validation
     **/

    // Not implemented yet
    // QString rrule;

    // If this line is reached, validation went well. No response to show.
    handleEventResponse("");
    emit eventAddEvent(summary, location, description, startDateTime, endDateTime);
}

Calendar *EventDialog::getCal() const
{
    return cal;
}

void EventDialog::setCal(Calendar *newCal)
{
    cal = newCal;
}

void EventDialog::handleEventResponse(QString errorMessage) {
    ui->errorMessageLabel->setText(errorMessage);
}

void EventDialog::setCalName(QString calName) {
    ui->calendarNameEdit->setText(calName);
}

void EventDialog::on_abortButton_clicked()
{
    emit closeEventDialog();
}


void EventDialog::on_EventDialog_rejected()
{
    emit closeEventDialog();
}

