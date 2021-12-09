#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Create the client that will handle the calendar list
    client = new Client();

    dialog = NULL;
    eventDialog = NULL;

    // This handle the updates on GUI of calendars available
    // TODO: check if it is possible to remove them from the variables of MainWindow
    _calBoxesLayout = new QVBoxLayout();
    _calBoxesLayout->setAlignment(Qt::AlignTop);
    _calBoxes = new QWidget(ui->scrollArea);
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
        connect(client, &Client::closeDialog, this, &MainWindow::handleAddCalendarFinished);
        connect(dialog, &Dialog::closeDialog, this, &MainWindow::handleCloseDialog);
        connect(client, &Client::printEvent, this, &MainWindow::handlePrintEvent);
        connect(client, &Client::refreshEventVisualization, this, &MainWindow::handleRefreshEventScrollArea);
    }

    dialog->setModal("true");
    dialog->show();
}

void MainWindow::handleCloseDialog() {
    dialog->hide();

    /**
     *  The alternative to disconnect is to DO NOT delete the dialog
     *  but clearing the line texts inside of it
     **/
    disconnect(dialog, &Dialog::eventAddCalendar, client, &Client::handleAddCalendar);
    disconnect(client, &Client::dialogErrorMessage, dialog, &Dialog::handleResponse);
    disconnect(client, &Client::closeDialog, this, &MainWindow::handleAddCalendarFinished);
    disconnect(dialog, &Dialog::closeDialog, this, &MainWindow::handleCloseDialog);

    //disconnect(client, &Client::printEvent, this, &MainWindow::handlePrintEvent);

    delete dialog;
    dialog = NULL;
}

void MainWindow::handlePrintEvent(QList<Event*> eventList){
    ui->eventScrollArea->widget()->hide();

//    ui->eventScrollArea->setStyleSheet(QString("color: white;"
//                                               "background-color: #8BC34A;"
//                                               "selection-color: yellow;"
//                                               "selection-background-color: blue;"
//                                               "background-attachment: scroll;"));

    QHBoxLayout* firstLine = new QHBoxLayout();
    QLabel* defaultText = new QLabel(QString("Sono presenti: " + QString::number(eventList.length())+" eventi per il calendario aggiunto"));
    firstLine->addWidget(defaultText);

    QVBoxLayout* fullBox = new QVBoxLayout();
    fullBox->setAlignment(Qt::AlignLeft);
    fullBox->addLayout(firstLine);

    for(int i =0; i< eventList.length(); i++) {
        QHBoxLayout* theLine = new QHBoxLayout();
        QPushButton* theText = new QPushButton(QString(eventList[i]->summary()));
        //I can't make clickable a QLabel... so I use a QPushButton
        //QLabel* theText = new QLabel(QString(eventList[i]->summary() +" "+ eventList[i]->location()));
        //theText->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
        QToolButton* editEvent = new QToolButton();
        editEvent->setText("Modifica");
        //editEvent->setIcon(QIcon::fromTheme("edit-copy"));
        QToolButton* deleteEvent = new QToolButton();
        //editEvent->setIcon(QIcon::fromTheme("edit-delete"));
        deleteEvent->setText("Cancella");
        QWidget* colourBox = new QWidget();
        colourBox->setStyleSheet(QString( "background-color: " + eventList[i]->colour() + ";" ));
        colourBox->setFixedSize(22, 22);
//        QPalette pal = QPalette();
//        pal.setColor(QPalette::Window, Qt::black);
//        fakeWidget->setAutoFillBackground(true);
//        fakeWidget->setPalette(pal);
//        fakeWidget->show();
        theLine->addWidget(colourBox);
        theLine->addWidget(theText);
        theLine->addWidget(editEvent);
        theLine->addWidget(deleteEvent);
        fullBox->addLayout(theLine);

        connect(theText, &QPushButton::clicked, eventList[i], &Event::showEvent);
        connect(editEvent, &QAbstractButton::clicked, eventList[i], &Event::showEvent);
        connect(deleteEvent, &QAbstractButton::clicked, eventList[i], &Event::handleRemoveEvent);
    }

    QWidget* eventBoxes = new QWidget(ui->eventScrollArea);
    QVBoxLayout* eventBoxesLayout = new QVBoxLayout();
    eventBoxesLayout->setAlignment(Qt::AlignTop);

    eventBoxesLayout->addLayout(fullBox);

    eventBoxes->setLayout(eventBoxesLayout);
    ui->eventScrollArea->setWidget(eventBoxes);

    ui->eventScrollArea->widget()->show();
}

void MainWindow::handleRefreshEventScrollArea(){
    handlePrintEvent(client->getEventByDate(ui->calendarWidget->selectedDate()));
}

void MainWindow::handleAddCalendarFinished(Calendar* cal) {
    handleCloseDialog();

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

    // The third line will show the buttons
    QHBoxLayout* thirdLine = new QHBoxLayout();
    //Add event button
    QPushButton* addEventButton = new QPushButton("Nuovo evento", this);
    // When "Add event" button is pressed call the calendar's slot that will call a slot to show EventDialog and refer to it
    connect(addEventButton, SIGNAL(clicked()), cal, SLOT(handleAddNewEventPopUp()));
    connect(cal,&Calendar::showEventDialog,this,&MainWindow::eventShowEventDialog);
    addEventButton->setMaximumSize(90,40);
    thirdLine->addWidget(addEventButton);

    //Remove calendar button
    QPushButton* removeButton = new QPushButton("Rimuovi Calendario", this);
    // When "Remove" button is pressed call the calendar's slot that will signal to the client/GUI that it has to be deleted
    connect(removeButton, SIGNAL(clicked()), cal, SLOT(handleRemoveCalendar()));
    // Mainwindow will update then the layout list
    connect(cal, &Calendar::removeCalendar, this, &MainWindow::handleRemoveCalendarBox);
    // Client will update the calendar's list
    connect(cal, &Calendar::removeCalendar, client, &Client::handleRemoveCalendarFromList);

    removeButton->setMaximumSize(140,40);
    thirdLine->addWidget(removeButton);

    //Box with the colour of the calendar
    QWidget* colourBox = new QWidget();
    colourBox->setStyleSheet(QString( "background-color: " + cal->colour() + ";" ));
    colourBox->setFixedSize(22, 22);
    thirdLine->addWidget(colourBox);

    thirdLine->setAlignment(Qt::AlignLeft);

    // Build now the whole box
    QVBoxLayout* fullBox = new QVBoxLayout();
    fullBox->setAlignment(Qt::AlignLeft);
    fullBox->addLayout(firstLine);
    fullBox->addLayout(secondLine);
    fullBox->addLayout(thirdLine);

    // There will be one "fullBox" for each calendar
    _calBoxesLayout->addLayout(fullBox);

    // Show the whole list to the UI
    _calBoxes->setLayout(_calBoxesLayout);
    //ui->scrollArea->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    ui->scrollArea->setWidget(_calBoxes);
    //ui->scrollArea->resize(_calBoxes->size().width(), ui->scrollAreaWidgetContents->size().height());


}


void MainWindow::on_calendarWidget_clicked(const QDate &date)
{ 
    handlePrintEvent(client->getEventByDate(date));
}


void MainWindow::handleRemoveCalendarBox(Calendar* cal) {
    // Temporary hide the content of the ScrollArea
    ui->scrollArea->widget()->hide();

    // Search for the calendar to be removed
    for(int i =0; i<_calBoxesLayout->count(); i++){
        QWidget *url = _calBoxesLayout->itemAt(i)->layout()->itemAt(1)->layout()->itemAt(1)->widget();
        QLabel *label = qobject_cast<QLabel *>(url);
//        qDebug() << "Url:" << label->text();
        if(cal->url() == label->text()){
            QLayoutItem * tobeRemoved = _calBoxesLayout->itemAt(i)->layout();
            _calBoxesLayout->removeItem(tobeRemoved);
            delete tobeRemoved;
        }
    }

    // Show the whole list to the UI
    QWidget* mybox = new QWidget();
    mybox->setLayout(_calBoxesLayout);
    delete _calBoxes;
    _calBoxes = mybox;
    ui->scrollArea->setWidget(mybox);

    // Show again scrollbar's content
    ui->scrollArea->widget()->show();
}

void MainWindow::eventShowEventDialog(Calendar* cal) {
    if(eventDialog == NULL) {
        eventDialog = new EventDialog();

        // Pair the eventDialog to the calendar
        eventDialog->setCal(cal);
        eventDialog->setCalName(cal->displayName());

//        connect(client, &Client::eventDialogErrorMessage, eventDialog, &EventDialog::handleEventResponse);

        connect(eventDialog, &EventDialog::eventAddEvent, cal, &Calendar::handleAddEvent);
        connect(eventDialog, &EventDialog::closeEventDialog, this, &MainWindow::handleAddEventFinished);
        connect(cal, &Calendar::eventAddFinished, this, &MainWindow::handleAddEventWithoutError);
        connect(cal, &Calendar::eventRetrieveError, this, &MainWindow::handleAddEventError);
    }

    eventDialog->setModal("true");
    eventDialog->show();
}
/**
 *
 * @brief Handle the closing of an EventDialog: for an event added succesfully, unsuccesfully or when the modal is closed by the proper button
 */
void MainWindow::handleAddEventFinished() {
    eventDialog->hide();

    /**
     *  The alternative to disconnect is to DO NOT delete the dialog
     *  but clearing the line texts inside of it
     **/
    disconnect(eventDialog, &EventDialog::eventAddEvent, eventDialog->getCal(), &Calendar::handleAddEvent);
    disconnect(eventDialog->getCal(), &Calendar::eventAddFinished, this, &MainWindow::handleAddEventFinished);

    delete eventDialog;
    eventDialog = NULL;
}

void MainWindow::handleAddEventError() {
    handleAddEventFinished();
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setText("L'evento NON è stato aggiunto correttamente");
    msgBox.exec();
}

void MainWindow::handleAddEventWithoutError() {
    handleAddEventFinished();
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setText("L'evento è stato aggiunto correttamente");
    msgBox.exec();
}

void MainWindow::on_calendarWidget_selectionChanged()
{

}

