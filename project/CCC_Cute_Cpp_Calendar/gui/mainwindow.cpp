#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    client = new Client();
    dialog = NULL;
    eventDialog = NULL;
    _calBoxes = new QWidget(ui->scrollArea);
    _calBoxesLayout = new QVBoxLayout();
    _calBoxesLayout->setAlignment(Qt::AlignTop);
}

MainWindow::~MainWindow()
{
    delete _calBoxesLayout;
    delete _calBoxes;
    delete ui;
    if(dialog != NULL)
        delete dialog;
    if(eventDialog != NULL)
        delete eventDialog;
    delete client;
}


void MainWindow::on_actionApri_calendario_triggered()
{
    if(dialog == NULL) {
        dialog = new Dialog();
        connect(dialog, &Dialog::eventAddCalendar, client, &Client::handleAddCalendar);
        connect(client, &Client::dialogErrorMessage, dialog, &Dialog::handleResponse);
        connect(client, &Client::closeDialog, this, &MainWindow::handleCloseDialog);
    }

    dialog->setModal("true");
    dialog->show();
}

void MainWindow::handleCloseDialog(Calendar* cal) {
    dialog->hide();
    disconnect(dialog, &Dialog::eventAddCalendar, client, &Client::handleAddCalendar);
    disconnect(client, &Client::dialogErrorMessage, dialog, &Dialog::handleResponse);
    disconnect(client, &Client::closeDialog, this, &MainWindow::handleCloseDialog);
    delete dialog;
    dialog = NULL;

    /**
     * In this phase there will be the creation of the box to be shown on the UI
     *
     **/

    // The first line will show the name of the calendar
    QHBoxLayout* firstLine = new QHBoxLayout();
    QLabel* name = new QLabel(cal->displayName());
    name->setStyleSheet("font-weight: bold;");
    firstLine->addWidget(name);

    // The second line will show the URL of the calendar
    QHBoxLayout* secondLine = new QHBoxLayout();
    QLabel* defaultUrl = new QLabel(QString("URL:"));
    secondLine->addWidget(defaultUrl);
    QLabel* urlCal = new QLabel(cal->url());
    secondLine->addWidget(urlCal);

    // The third line will show the button to remove the calendar
    QHBoxLayout* thirdLine = new QHBoxLayout();
    QPushButton* removeButton = new QPushButton("Rimuovi", this);
    thirdLine->addWidget(removeButton);

    // Build now the whole box
    QVBoxLayout* fullBox = new QVBoxLayout();
    fullBox->setAlignment(Qt::AlignLeft);
    fullBox->addLayout(firstLine);
    fullBox->addLayout(secondLine);
    fullBox->addLayout(thirdLine);

    // Add the new box to the map
    _boxesMap.insert(cal->displayName(),fullBox);

    // Refresh
//    QVBoxLayout* calBoxesLayout = new QVBoxLayout(_calBoxes);
//    calBoxesLayout->setAlignment(Qt::AlignTop);

//    QMap<QString,QVBoxLayout*>::iterator it;
//    for (it = _boxesMap.begin(); it != _boxesMap.end(); it++) {
//        calBoxesLayout->addLayout(it.value());
//        qDebug() << "Itero";
//    }

    _calBoxesLayout->addLayout(fullBox);

    // Show the whole list to the UI
    _calBoxes->setLayout(_calBoxesLayout);
    ui->scrollArea->setWidget(_calBoxes);
}

void MainWindow::on_scrollArea_customContextMenuRequested(const QPoint &pos)
{

}


void MainWindow::on_calendarWidget_clicked(const QDate &date)
{
    //TODO: User selected a date, show Events and ToDo for that date
}


void MainWindow::on_pushButton_clicked()
{
    if(eventDialog == NULL) {
        eventDialog = new EventDialog();
        connect(eventDialog, &EventDialog::eventAddEvent, client, &Client::handleAddEvent);
        connect(client, &Client::eventDialogErrorMessage, eventDialog, &EventDialog::handleEventResponse);
        connect(client, &Client::closeEventDialog, this, &MainWindow::handleCloseEventDialog);
    }

    eventDialog->setModal("true");
    eventDialog->show();
}

//TODO: implement this
void MainWindow::handleCloseEventDialog(Event* event) {

}

