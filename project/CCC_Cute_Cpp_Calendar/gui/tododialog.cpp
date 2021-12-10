#include "tododialog.h"
#include "ui_tododialog.h"

TodoDialog::TodoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TodoDialog)
{
    ui->setupUi(this);

    ui->startingHoursLine->setText(QString("00"));
    ui->startingMinutesLine->setText(QString("00"));

    ui->startingHoursLine->setValidator(new QIntValidator(0, 23, ui->startingHoursLine));
    ui->startingMinutesLine->setValidator(new QIntValidator(0, 59, ui->startingMinutesLine));
}

TodoDialog::~TodoDialog()
{
    delete ui;
}

Calendar *TodoDialog::getCal() const
{
    return cal;
}

void TodoDialog::setCal(Calendar *newCal)
{
    cal = newCal;
}

void TodoDialog::setCalName(QString calName) {
    ui->calendarNameEdit->setText(calName);
}

Todo *TodoDialog::getTodo() const
{
    return todo;
}

void TodoDialog::setTodo(Todo *newTodo)
{
    todo = newTodo;
}

void TodoDialog::setFields() {

    ui->summaryLine->setText(todo->summary());
    ui->calendarWidgetDate->setSelectedDate(todo->dueDateTime().date());
    ui->uidLine->setText(todo->uid());

    int startingHours = todo->dueDateTime().time().hour();
    QString stH;
    if(startingHours < 10) {
        stH = "0" + QString::fromStdString(std::to_string(startingHours));
    } else {
        stH = QString::fromStdString(std::to_string(startingHours));
    }

    int startingMinutes = todo->dueDateTime().time().minute();
    QString stM;
    if(startingMinutes < 10) {
        stM = "0" + QString::fromStdString(std::to_string(startingMinutes));
    } else {
        stM = QString::fromStdString(std::to_string(startingMinutes));
    }



    ui->startingHoursLine->setText(stH);
    ui->startingMinutesLine->setText(stM);
}

void TodoDialog::on_confirmButton_clicked()
{
    // Summary field & check
    QString summary = QString(ui->summaryLine->text());

    if(summary.length()<1) {
        handleTodoResponse(QString("Il To-Do deve avere un nome."));
        return;
    }

    QDate startingDate = ui->calendarWidgetDate->selectedDate();

    /**
     * Validation of Date and Time received from GUI
     **/
    if(startingDate.isNull()) {
        handleTodoResponse(QString("Devi selezionare correttamente una data per il To-Do."));
        return;
    }

    QDateTime startDateTime;

    startDateTime.setDate(startingDate);

    if(ui->startingHoursLine->text().isNull() || ui->startingMinutesLine->text().isNull() ||
            ui->startingHoursLine->text().length() == 0 || ui->startingMinutesLine->text().length() == 0) {
        handleTodoResponse(QString("I campi orario non possono essere vuoti."));
        return;
    }
    int startingHours = ui->startingHoursLine->text().toInt();

    if(startingHours < 0 || startingHours > 24) {
        handleTodoResponse(QString("Ora di inizio non corretta."));
        return;
    }

    int startingMinutes = ui->startingMinutesLine->text().toInt();

    if(startingMinutes < 0 || startingMinutes > 59) {
        handleTodoResponse(QString("Minuti di inizio non corretti."));
        return;
    }

    startDateTime.setTime(QTime(startingHours,startingMinutes));


    /**
     * End of Date and Time validation
     **/


    // If this line is reached, validation went well. No response to show.
    handleTodoResponse("");
    emit eventAddTodo(summary, startDateTime);
}

void TodoDialog::handleTodoResponse(QString errorMessage) {
    ui->errorMessageLabel->setText(errorMessage);
}


void TodoDialog::on_abortButton_clicked()
{
    emit closeTodoDialog();
}

void TodoDialog::on_TodoDialog_rejected()
{
    emit closeTodoDialog();
}

