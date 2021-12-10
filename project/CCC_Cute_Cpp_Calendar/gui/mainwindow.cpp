#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Create the client that will handle the calendar list
    client = new Client();
    connect (client, &Client::updateMainWindow, this, &MainWindow::handleUpdateMainWindowWidgets);

    dialog = NULL;
    eventDialog = NULL;

    // This handle the updates on GUI of calendars available
    // TODO: check if it is possible to remove them from the variables of MainWindow
    _calBoxesLayout = new QVBoxLayout();
    _calBoxesLayout->setAlignment(Qt::AlignTop);
    _calBoxes = new QWidget(ui->scrollArea);

    handleUpdateMainWindowWidgets();
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
        connect(dialog, &Dialog::closeDialog, this, &MainWindow::handleCloseDialog);/*
        connect(client, &Client::printEvent, this, &MainWindow::handlePrintEvent);*/
        connect(client, &Client::refreshEventVisualization, this, &MainWindow::handleUpdateMainWindowWidgets);
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

void MainWindow::handleAddCalendarFinished(Calendar* cal) {
    handleCloseDialog();
    handleUpdateMainWindowWidgets();
}


void MainWindow::on_calendarWidget_clicked(const QDate &date)
{ 
    handleUpdateMainWindowWidgets();
}


void MainWindow::handleRemoveCalendarBox(Calendar* cal) {
    handleUpdateMainWindowWidgets();
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


void MainWindow::handleUpdateMainWindowWidgets() {
    /****** Starting GUI update based on calendars available ******/

    int numberOfCalendars = client->getCalendarCount();

    if(numberOfCalendars == 0) {
        printEmptyCalendars();
        printEmptyEvents();
        return;
    }

    // If i'm here there is at least one calendar
    printCalendars();

    /****** End of GUI update based on calendars available ******/

    /****** Starting GUI update based on events available ******/
    QList <Event*> eventsList = client->getEventByDate(ui->calendarWidget->selectedDate());
    int numberOfEvents = eventsList.count();



    if(numberOfEvents == 0) {
        printEmptyEvents();
        return;
    }

    // I'f i'm here there is at least one event
    printEventsList(eventsList);

    /****** End of GUI update based on events available ******/
}

void MainWindow::printEmptyCalendars() {
    // Show message on screen that no calendars are available
    QVBoxLayout* firstLine = new QVBoxLayout();
    firstLine->setAlignment(Qt::AlignVCenter);

    QLabel* firstLabel = new QLabel("Nessun calendario.");
    QLabel* secondLabel = new QLabel("Clicca su File per iniziare!");
    firstLabel->setStyleSheet("font-weight: bold;");
    firstLine->addWidget(firstLabel);
    firstLine->addWidget(secondLabel);


    QWidget* calBox = new QWidget(ui->scrollArea);
    calBox->setLayout(firstLine);
    ui->scrollArea->setWidget(calBox);

    // Disable calendar
    ui->calendarWidget->setDisabled(true);
}

void MainWindow::printCalendars() {
    // Enable calendar
    ui->calendarWidget->setDisabled(false);

    // Print calendars list

    // Prepare the widget that will contain the list and its layout
    QWidget* calBoxes = new QWidget(ui->scrollArea);
    QVBoxLayout* calBoxesLayout = new QVBoxLayout();
    calBoxesLayout->setAlignment(Qt::AlignTop);

    for(Calendar* cal : client->calendarList()) {

        // The first line will show the name+colour of the calendar
        QHBoxLayout* firstLine = new QHBoxLayout();

        //Box with the colour of the calendar
        QWidget* colourBox = new QWidget();
        colourBox->setStyleSheet(QString( "background-color: " + cal->colour() + ";" ));
        colourBox->setFixedSize(20, 20);

        // Round the box and make it circle
        QPainterPath path;
        path.addRoundedRect(colourBox->rect(), 10, 10);
        QRegion mask = QRegion(path.toFillPolygon().toPolygon());
        colourBox->setMask(mask);

        firstLine->addWidget(colourBox);

        QLabel* name = new QLabel(cal->displayName());
        name->setStyleSheet("font-weight: bold;");
        firstLine->addWidget(name);
        firstLine->setAlignment(Qt::AlignLeft);

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
        // Client will update the calendar's list
        connect(cal, &Calendar::removeCalendar, client, &Client::handleRemoveCalendarFromList);

        removeButton->setMaximumSize(140,40);
        thirdLine->addWidget(removeButton);

        thirdLine->setAlignment(Qt::AlignLeft);

        // Build now the whole box
        QVBoxLayout* fullBox = new QVBoxLayout();
        fullBox->setAlignment(Qt::AlignLeft);
        fullBox->addLayout(firstLine);
        fullBox->addLayout(secondLine);
        fullBox->addLayout(thirdLine);

        // There will be one "fullBox" for each calendar
        calBoxesLayout->addLayout(fullBox);

    }

    // Show the whole list to the UI
    calBoxes->setLayout(calBoxesLayout);
    //ui->scrollArea->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    ui->scrollArea->setWidget(calBoxes);
    //ui->scrollArea->resize(_calBoxes->size().width(), ui->scrollAreaWidgetContents->size().height());
}

void MainWindow::printEmptyEvents() {
    // Show message on screen that no events are available
    QVBoxLayout* firstLine = new QVBoxLayout();
    firstLine->setAlignment(Qt::AlignVCenter);

    QLabel* firstLabel = new QLabel("Nessun evento disponibile.");
    firstLabel->setStyleSheet("font-weight: bold;");
    firstLine->addWidget(firstLabel);

    QWidget* calBox = new QWidget(ui->eventScrollArea);
    calBox->setLayout(firstLine);
    ui->eventScrollArea->setWidget(calBox);
}

void MainWindow::printEventsList(QList <Event*> eventsList) {
    // Prepare the widget that will contain the list and its layout
    QWidget* evBoxes = new QWidget(ui->eventScrollArea);

    QHBoxLayout* firstLine = new QHBoxLayout();
    QLabel* defaultText = new QLabel(QString("Sono presenti: " + QString::number(eventsList.length())+" eventi per la data selezionata"));
    firstLine->addWidget(defaultText);

    QVBoxLayout* fullBox = new QVBoxLayout();
    fullBox->setAlignment(Qt::AlignTop);
    fullBox->addLayout(firstLine);

    for(int i =0; i< eventsList.length(); i++) {
        QHBoxLayout* theLine = new QHBoxLayout();
        QPushButton* theText = new QPushButton(QString(eventsList[i]->summary()));
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

        colourBox->setStyleSheet(QString( "background-color: " + eventsList[i]->colour() + ";" ));
        colourBox->setFixedSize(20, 20);

        // Make it rounded
        QPainterPath path;
        path.addRoundedRect(colourBox->rect(), 10, 10);
        QRegion mask = QRegion(path.toFillPolygon().toPolygon());
        colourBox->setMask(mask);

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

        connect(theText, &QPushButton::clicked, eventsList[i], &Event::showEvent);
        connect(editEvent, &QAbstractButton::clicked, eventsList[i], &Event::showEvent);
        connect(deleteEvent, &QAbstractButton::clicked, eventsList[i], &Event::handleRemoveEvent);
    }

    evBoxes->setLayout(fullBox);
    ui->eventScrollArea->setWidget(evBoxes);
}
