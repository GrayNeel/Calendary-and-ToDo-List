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

    if(updating)
        emit eventModifyEvent(event->uid(), summary, location, description, startDateTime, endDateTime);
    else
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

void EventDialog::setUpdating(bool value)
{
    updating = value;
}

Event *EventDialog::getEvent() const
{
    return event;
}

void EventDialog::setEvent(Event *newEvent)
{
    event = newEvent;
}

void EventDialog::setFields() {

    ui->summaryLine->setText(event->summary());
    ui->locationLine->setText(event->location());
    ui->descriptionText->setText(event->description());
    ui->calendarWidgetStart->setSelectedDate(event->startDateTime().date());
    ui->calendarWidgetEnd->setSelectedDate(event->endDateTime().date());
    ui->uidLine->setText(event->uid());

    int startingHours = event->startDateTime().time().hour();
    QString stH;
    if(startingHours < 10) {
        stH = "0" + QString::fromStdString(std::to_string(startingHours));
    } else {
        stH = QString::fromStdString(std::to_string(startingHours));
    }

    int startingMinutes = event->startDateTime().time().minute();
    QString stM;
    if(startingMinutes < 10) {
        stM = "0" + QString::fromStdString(std::to_string(startingMinutes));
    } else {
        stM = QString::fromStdString(std::to_string(startingMinutes));
    }

    int endingHours = event->endDateTime().time().hour();
    QString enH;
    if(endingHours < 10) {
        enH = "0" + QString::fromStdString(std::to_string(endingHours));
    } else {
        enH = QString::fromStdString(std::to_string(endingHours));
    }


    int endingMinutes = event->endDateTime().time().minute();
    QString enM;
    if(endingMinutes < 10) {
        enM = "0" + QString::fromStdString(std::to_string(endingMinutes));
    } else {
        enM = QString::fromStdString(std::to_string(endingMinutes));
    }

    ui->startingHoursLine->setText(stH);
    ui->startingMinutesLine->setText(stM);

    ui->endingHoursLine->setText(enH);
    ui->endingMinutesLine->setText(enM);
}

void EventDialog::disableFields(bool status) {

    ui->summaryLine->setDisabled(status);
    ui->locationLine->setDisabled(status);
    ui->descriptionText->setDisabled(status);
    ui->calendarWidgetStart->setDisabled(status);
    ui->calendarWidgetEnd->setDisabled(status);
    ui->uidLine->setDisabled(true);

    ui->startingHoursLine->setDisabled(status);
    ui->startingMinutesLine->setDisabled(status);

    ui->endingHoursLine->setDisabled(status);
    ui->endingMinutesLine->setDisabled(status);
}


