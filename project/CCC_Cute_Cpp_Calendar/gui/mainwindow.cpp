#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Create the client that will handle the calendar list
    client = new Client(this);
    // Create a connection that will help client communicate to GUI that there is an update
    connect (client, &Client::closeDialog, this, &MainWindow::handleCloseDialog);
    connect (client, &Client::updateMainWindow, this, &MainWindow::handleUpdateMainWindowWidgets);
    connect (this, &MainWindow::refreshCalendars, client, &Client::handleRefreshCalendars);

    dialog = NULL;
    eventDialog = NULL;
    todoDialog = NULL;

    // Set update icon
    ui->updateButton->setIcon(QIcon("updateIcon.svg"));
    ui->updateButton->setIconSize(QSize(20, 20));

    // Set initial state of GUI    
    handleUpdateMainWindowWidgets();
}

MainWindow::~MainWindow()
{
    delete ui;
    if(dialog != NULL)
        delete dialog;
    if(eventDialog != NULL)
        delete eventDialog;
    delete client;
}

/** Actions from GUI objects **/

void MainWindow::on_actionApri_calendario_triggered()
{
    if(dialog == NULL) {
        dialog = new Dialog(this);

        // Events that happen when calendar is retrieved from server (on button click)
        connect(dialog, &Dialog::eventAddCalendar, client, &Client::handleAddCalendar);
        connect(client, &Client::dialogErrorMessage, dialog, &Dialog::handleResponse);

        // Events that happen when add calendar dialog is being closed
        connect(dialog, &Dialog::closeDialog, this, &MainWindow::handleUpdateMainWindowWidgets);
        connect(dialog, &Dialog::closeDialog, this, &MainWindow::handleCloseDialog);
    }

    dialog->setModal("true");
    dialog->show();
}

void MainWindow::on_calendarWidget_clicked(const QDate &date)
{
    // Update events for that date (the date is retrieved inside the function)
    handleUpdateMainWindowWidgets();
}

/** Custom Slots **/

/**
 * @brief Handles the close of the Add Calendar Dialog and its deletion
 */
void MainWindow::handleCloseDialog() {
    if(dialog!=NULL)
    {
        dialog->hide();

        /**
     *  The alternative to disconnect is to DO NOT delete the dialog
     *  but clearing the line texts inside of it
     **/
        disconnect(dialog, &Dialog::eventAddCalendar, client, &Client::handleAddCalendar);
        disconnect(client, &Client::dialogErrorMessage, dialog, &Dialog::handleResponse);
        disconnect(dialog, &Dialog::closeDialog, this, &MainWindow::handleUpdateMainWindowWidgets);
        disconnect(dialog, &Dialog::closeDialog, this, &MainWindow::handleCloseDialog);

        delete dialog;
        dialog = NULL;
    }
}

/**
 * @brief Show the Event Dialog which is paired to the specific calendar from which the button has been clicked.
 * @param cal the calendar from which the button has been clicked.
 */
void MainWindow::eventShowEventDialog(Calendar* cal) {
    if(eventDialog == NULL) {
        eventDialog = new EventDialog(this);

        // Pair the eventDialog to the calendar
        eventDialog->setCal(cal);
        eventDialog->setCalName(cal->displayName());

        // Events that happen when event is added (on button click)
        connect(eventDialog, &EventDialog::eventAddEvent, cal, &Calendar::handleAddEvent);
        connect(eventDialog, &EventDialog::closeEventDialog, this, &MainWindow::handleAddEventFinished);
        connect(cal, &Calendar::eventAddFinished, this, &MainWindow::handleAddEventWithoutError);
        connect(cal, &Calendar::eventRetrieveError, this, &MainWindow::handleAddEventError);
    }

    eventDialog->setUpdating(false);

    eventDialog->setModal("true");
    eventDialog->show();
}


/**
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
    disconnect(eventDialog->getCal(), &Calendar::eventAddFinished, this, &MainWindow::handleAddEventWithoutError);
    disconnect(eventDialog->getCal(), &Calendar::eventRetrieveError, this, &MainWindow::handleAddEventError);

    delete eventDialog;
    eventDialog = NULL;
}

/**
 * @brief Shows a popup message that event has not been successfully added
 */
void MainWindow::handleAddEventError() {
    handleAddEventFinished();
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setText("L'evento NON è stato aggiunto correttamente");
    msgBox.exec();
}

/**
 * @brief Shows a popup message that event has been successfully added
 */
void MainWindow::handleAddEventWithoutError() {
    handleAddEventFinished();
    handleUpdateMainWindowWidgets();
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setText("L'evento è stato aggiunto correttamente");
    msgBox.exec();
}

/**
 * @brief Show the Todo Dialog which is paired to the specific calendar from which the button has been clicked.
 * @param cal the calendar from which the button has been clicked.
 */
void MainWindow::eventShowTodoDialog(Calendar* cal) {
    if(todoDialog == NULL) {
        todoDialog = new TodoDialog(this);

        // Pair the todoDialog to the calendar
        todoDialog->setCal(cal);
        todoDialog->setCalName(cal->displayName());

        // Events that happen when todo is added (on button click)
        connect(todoDialog, &TodoDialog::eventAddTodo, cal, &Calendar::handleAddTodo);
        connect(todoDialog, &TodoDialog::closeTodoDialog, this, &MainWindow::handleAddTodoFinished);
        connect(cal, &Calendar::todoAddFinished, this, &MainWindow::handleAddTodoWithoutError);
        connect(cal, &Calendar::todoRetrieveError, this, &MainWindow::handleAddTodoError);
    }

    todoDialog->setUpdating(false);

    todoDialog->setModal("true");
    todoDialog->show();
}

/**
 * @brief Handle the closing of an EventDialog: for an event added succesfully, unsuccesfully or when the modal is closed by the proper button
 */
void MainWindow::handleAddTodoFinished() {
    todoDialog->hide();

    /**
     *  The alternative to disconnect is to DO NOT delete the dialog
     *  but clearing the line texts inside of it
     **/
    disconnect(todoDialog, &TodoDialog::eventAddTodo, todoDialog->getCal(), &Calendar::handleAddTodo);
    disconnect(todoDialog, &TodoDialog::closeTodoDialog, this, &MainWindow::handleAddTodoFinished);
    disconnect(todoDialog->getCal(), &Calendar::todoAddFinished, this, &MainWindow::handleAddTodoFinished);
    disconnect(todoDialog->getCal(), &Calendar::todoAddFinished, this, &MainWindow::handleAddTodoWithoutError);
    disconnect(todoDialog->getCal(), &Calendar::todoRetrieveError, this, &MainWindow::handleAddTodoError);

    delete todoDialog;
    todoDialog = NULL;
}

/**
 * @brief Shows a popup message that event has not been successfully added
 */
void MainWindow::handleAddTodoError() {
    handleAddTodoFinished();
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setText("Il To-Do NON è stato aggiunto correttamente");
    msgBox.exec();
}

/**
 * @brief Shows a popup message that event has been successfully added
 */
void MainWindow::handleAddTodoWithoutError() {
    handleAddTodoFinished();
    handleUpdateMainWindowWidgets();
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setText("Il To-Do è stato aggiunto correttamente");
    msgBox.exec();
}


void MainWindow::handleShowInfoEventDialog(Event *event)
{
    if(eventDialog == NULL) {
        eventDialog = new EventDialog(this);

        QString calName;
        Calendar* calToPair;
        client->calendarList();

        // Search for calendar to which event is connected to
        for(Calendar* cal : client->calendarList()) {
            for(Event* ev : cal->eventsList()) {
                if(ev->uid() == event->uid()) {
                    calName = cal->displayName();
                    calToPair = cal;
                    break;
                }
            }
        }

        // Pair the eventDialog to the event
        eventDialog->setEvent(event);
        eventDialog->setCal(calToPair);
        eventDialog->setCalName(calName);

        // Prepare fields
        eventDialog->setFields();
        eventDialog->disableFields(true);

        // Events that happen when event is modified (on button click)
        connect(eventDialog, &EventDialog::closeEventDialog, this, &MainWindow::handleCloseModifyEventDialog);


        QHBoxLayout *hbox = eventDialog->findChild<QVBoxLayout*>("_2")->findChild<QHBoxLayout*>("horizontalLayout");

        QPushButton* confirm = static_cast<QPushButton*> (hbox->itemAt(1)->widget());
        confirm->setDisabled(true);
        QPushButton* editButton = new QPushButton("Modifica");
        connect(editButton, &QPushButton::clicked, this, &MainWindow::handleShowModifyEventDialog);
        hbox->insertWidget(3, editButton);
    }

    eventDialog->setModal("true");
    eventDialog->show();

}

/**
 * @brief This function shows the eventDialog filled of event parameters ready to be modified
 * @param event the event to be modified
 */
void MainWindow::handleShowModifyEventDialog() {

        eventDialog->setUpdating(true);

        // Prepare fields
        eventDialog->disableFields(false);

        // Events that happen when event is modified (on button click)
        connect(eventDialog, &EventDialog::eventModifyEvent, eventDialog->getCal(), &Calendar::handleModifyEvent);
        connect(eventDialog->getCal(), &Calendar::eventModifyFinished, this, &MainWindow::handleModifyEventWithoutError);
        connect(eventDialog->getCal(), &Calendar::eventModifyRetrieveError, this, &MainWindow::handleModifyEventError);

        //originali
        //connect(cal, &Calendar::eventAddFinished, this, &MainWindow::handleAddEventWithoutError);
        //connect(cal, &Calendar::eventRetrieveError, this, &MainWindow::handleAddEventError);

    QHBoxLayout *hbox = eventDialog->findChild<QVBoxLayout*>("_2")->findChild<QHBoxLayout*>("horizontalLayout");
    //Rendo di nuovo cliccabile pulsante "Conferma"
    QPushButton* confirm = static_cast<QPushButton*> (hbox->itemAt(1)->widget());
    confirm->setDisabled(false);
    //Disattivo pulsante "Modifica"
    QPushButton* edit = static_cast<QPushButton*> (hbox->itemAt(3)->widget());
    edit->setDisabled(true);
}

/**
 * @brief Handle the closing of an EventDialog opened to modify event: for an event edite succesfully, unsuccesfully or when the modal is closed by the proper button
 */
void MainWindow::handleCloseModifyEventDialog() {
    eventDialog->hide();

    /**
     *  The alternative to disconnect is to DO NOT delete the dialog
     *  but clearing the line texts inside of it
     **/
    disconnect(eventDialog, &EventDialog::closeEventDialog, this, &MainWindow::handleCloseModifyEventDialog);

    disconnect(eventDialog, &EventDialog::eventModifyEvent, eventDialog->getCal(), &Calendar::handleModifyEvent);
    disconnect(eventDialog->getCal(), &Calendar::eventModifyFinished, this, &MainWindow::handleModifyEventWithoutError);
    disconnect(eventDialog->getCal(), &Calendar::eventModifyRetrieveError, this, &MainWindow::handleModifyEventError);

    delete eventDialog;
    eventDialog = NULL;
}

/**
 * @brief Shows a popup message that event has not been successfully added
 */
void MainWindow::handleModifyEventError() {
    handleCloseModifyEventDialog();
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setText("L'evento NON è stato aggiornato correttamente");
    msgBox.exec();
}

/**
 * @brief Shows a popup message that event has been successfully added
 */
void MainWindow::handleModifyEventWithoutError() {
    handleCloseModifyEventDialog();
    handleUpdateMainWindowWidgets();
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setText("L'evento è stato aggiornato correttamente");
    msgBox.exec();
}





void MainWindow::handleShowInfoTodoDialog(Todo* todo) {
    if(todoDialog == NULL) {
        todoDialog = new TodoDialog(this);

        Calendar* calToPair;
        QString calName;
        client->calendarList();

        // Search for calendar to which event is connected to
        for(Calendar* cal : client->calendarList()) {
            for(Todo* td : cal->todosList()) {
                if(td->uid() == todo->uid()) {
                    calName = cal->displayName();
                    calToPair = cal;
                    break;
                }
            }
        }

        // Pair the eventDialog to the event
        todoDialog->setTodo(todo);
        todoDialog->setCal(calToPair);
        todoDialog->setCalName(calName);

        // Prepare fields
        todoDialog->setFields();
        todoDialog->disableFields(true);

        // Events that happen when event is modified (on button click)
        connect(todoDialog, &TodoDialog::closeTodoDialog, this, &MainWindow::handleCloseModifyTodoDialog);

        QHBoxLayout *hbox = todoDialog->findChild<QVBoxLayout*>("verticalLayout")->findChild<QVBoxLayout*>("verticalLayout_2")->findChild<QHBoxLayout*>("horizontalLayout_5");
        QPushButton* confirm = static_cast<QPushButton*> (hbox->itemAt(1)->widget());
        confirm->setDisabled(true);
        QPushButton* editButton = new QPushButton("Modifica");
        connect(editButton, &QPushButton::clicked, this, &MainWindow::handleShowModifyTodoDialog);
        hbox->insertWidget(3, editButton);
    }


    todoDialog->setModal("true");
    todoDialog->show();
}

/**
 * @brief This function shows the eventDialog filled of event parameters ready to be modified
 * @param event the event to be modified
 */
void MainWindow::handleShowModifyTodoDialog() {

        todoDialog->setUpdating(true);
        // Prepare fields
        todoDialog->setFields();
        todoDialog->disableFields(false);

        // Events that happen when event is modified (on button click)
        connect(todoDialog, &TodoDialog::eventModifyTodo, todoDialog->getCal(), &Calendar::handleModifyTodo);
        connect(todoDialog->getCal(), &Calendar::todoModifyFinished, this, &MainWindow::handleModifyTodoWithoutError);
        connect(todoDialog->getCal(), &Calendar::todoModifyRetrieveError, this, &MainWindow::handleModifyTodoError);


        QHBoxLayout *hbox = todoDialog->findChild<QVBoxLayout*>("verticalLayout")->findChild<QVBoxLayout*>("verticalLayout_2")->findChild<QHBoxLayout*>("horizontalLayout_5");
        //Rendo di nuovo cliccanile pulsante "Conferma"
        QPushButton* confirm = static_cast<QPushButton*> (hbox->itemAt(1)->widget());
        confirm->setDisabled(false);
        //Rimuovo pulsante "Modifica"
        QPushButton* edit = static_cast<QPushButton*> (hbox->itemAt(3)->widget());
        edit->setDisabled(true);
}

void MainWindow::handleCloseModifyTodoDialog() {
    todoDialog->hide();

    /**
     *  The alternative to disconnect is to DO NOT delete the dialog
     *  but clearing the line texts inside of it
     **/
    disconnect(todoDialog, &TodoDialog::closeTodoDialog, this, &MainWindow::handleCloseModifyTodoDialog);

    disconnect(todoDialog, &TodoDialog::eventModifyTodo, todoDialog->getCal(), &Calendar::handleModifyTodo);
    disconnect(todoDialog->getCal(), &Calendar::todoModifyFinished, this, &MainWindow::handleModifyTodoWithoutError);
    disconnect(todoDialog->getCal(), &Calendar::todoModifyRetrieveError, this, &MainWindow::handleModifyTodoError);

    delete todoDialog;
    todoDialog = NULL;
}

/**
 * @brief Shows a popup message that event has not been successfully added
 */
void MainWindow::handleModifyTodoError() {
    handleCloseModifyTodoDialog();
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setText("Il To-Do NON è stato aggiornato correttamente");
    msgBox.exec();
}

/**
 * @brief Shows a popup message that event has been successfully added
 */
void MainWindow::handleModifyTodoWithoutError() {
    handleCloseModifyTodoDialog();
    handleUpdateMainWindowWidgets();
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setText("Il To-Do è stato aggiornato correttamente");
    msgBox.exec();
}


/**
 * @brief Handle the update of the whole GUI: calendars, events and TODOs
 */
void MainWindow::handleUpdateMainWindowWidgets() {
    /****** Starting GUI update based on calendars available ******/

    int numberOfCalendars = client->getCalendarCount();

    if(numberOfCalendars == 0) {
        printEmptyCalendars();
        printEmptyEvents();
        printEmptyTodos();
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
    } else {
        // I'f i'm here there is at least one event
        printEventsList(eventsList);
    }

    /****** End of GUI update based on events available ******/

    /****** Starting GUI update based on TODOs available ******/

    QList <Todo*> todosList = client->getTodoByDate(ui->calendarWidget->selectedDate());
    int numberOfTodos = todosList.count();

    if(numberOfTodos == 0) {
        printEmptyTodos();
    } else {
        // I'f i'm here there is at least one event
        printTodosList(todosList);
    }
    /****** End of GUI update based on TODOs available ******/
}

/** Private functions for GUI update **/

void MainWindow::printEmptyCalendars() {
    // Show message on screen that no calendars are available
    QVBoxLayout* firstLine = new QVBoxLayout();
    firstLine->setAlignment(Qt::AlignVCenter);

    QLabel* firstLabel = new QLabel("Nessun calendario.");
    QLabel* secondLabel = new QLabel("Clicca su Apri per iniziare!");
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
        QPushButton* addEventButton = new QPushButton("", this);
        addEventButton->setIcon(QIcon("newEventIcon.svg"));
        addEventButton->setIconSize(QSize(20, 20));
        // When "Add event" button is pressed call the calendar's slot that will call a slot to show EventDialog and refer to it
        connect(addEventButton, SIGNAL(clicked()), cal, SLOT(handleAddNewEventPopUp()));
        connect(cal,&Calendar::showEventDialog,this,&MainWindow::eventShowEventDialog);
        addEventButton->setMaximumSize(90,40);
        thirdLine->addWidget(addEventButton);

        //Add todo button
        QPushButton* addTodoButton = new QPushButton("", this);
        addTodoButton->setIcon(QIcon("newTodoIcon.svg"));
        addTodoButton->setIconSize(QSize(20, 20));
        // When "Add ToDo" button is pressed call the calendar's slot that will call a slot to show TodoDialog and refer to it
        connect(addTodoButton, SIGNAL(clicked()), cal, SLOT(handleAddNewTodoPopUp()));
        connect(cal,&Calendar::showTodoDialog,this,&MainWindow::eventShowTodoDialog);
        addTodoButton->setMaximumSize(90,40);
        thirdLine->addWidget(addTodoButton);

        //Remove calendar button
        QPushButton* removeButton = new QPushButton("", this);
        removeButton->setIcon(QIcon("deleteIcon.svg"));
        removeButton->setIconSize(QSize(20, 20));
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
    QLabel* defaultText = new QLabel(QString("Eventi per il " + ui->calendarWidget->selectedDate().toString()));
    defaultText->setStyleSheet("font-weight: bold;");
    firstLine->addWidget(defaultText);

    QVBoxLayout* fullBoxes = new QVBoxLayout();
    fullBoxes->setAlignment(Qt::AlignTop);
    fullBoxes->addLayout(firstLine);

    for(int i =0; i< eventsList.length(); i++) {
        //First line contains color + name
        QHBoxLayout* firstLine = new QHBoxLayout();
        // Creating colour box
        QWidget* colourBox = new QWidget();

        colourBox->setStyleSheet(QString( "background-color: " + eventsList[i]->colour() + ";" ));
        colourBox->setFixedSize(20, 20);

        // Make it rounded
        QPainterPath path;
        path.addRoundedRect(colourBox->rect(), 10, 10);
        QRegion mask = QRegion(path.toFillPolygon().toPolygon());
        colourBox->setMask(mask);

        // Creating event name
        QLabel* eventName = new QLabel(QString(eventsList[i]->summary()));
        eventName->setStyleSheet("font-weight: bold;");

        firstLine->addWidget(colourBox);
        firstLine->addWidget(eventName);

        //Second line contains starting - ending time
        QHBoxLayout* secondLine = new QHBoxLayout();
        secondLine->setAlignment(Qt::AlignLeft);

        QDateTime startDateTime = eventsList[i]->startDateTime();
        QDateTime endDateTime = eventsList[i]->endDateTime();

        QLabel* time;

        // If starting date and ending date is equal print both starting and ending time
        if(startDateTime.date() == endDateTime.date()) {
            time = new QLabel(QString(startDateTime.time().toString("hh:mm") + " - " + endDateTime.time().toString("hh:mm")));
            time->setStyleSheet("font-weight: italic;");
        } else {
            if (ui->calendarWidget->selectedDate() == startDateTime.date())
                // If selectedDate == startDateTime show only starting time
                time = new QLabel(QString(startDateTime.time().toString("hh:mm") + " - "));
            else if (ui->calendarWidget->selectedDate() == endDateTime.date())
                // If selectedDate == endDateTime show only ending time
                time = new QLabel(QString(" - " + endDateTime.time().toString("hh:mm")));
            else if (ui->calendarWidget->selectedDate() > startDateTime.date() && ui->calendarWidget->selectedDate() < endDateTime.date()) {
                // If selectedDate is between starting and ending date show only an alert that event lasts all day
                time = new QLabel(QString("Tutto il giorno"));
            } else {
                // Should not happen
                time = new QLabel(QString("Invalid date"));
            }
        }

        time->setStyleSheet("font-style: italic;");


        QPushButton* infoEvent = new QPushButton();

        infoEvent->setIcon(QIcon("editIcon.svg"));
        infoEvent->setIconSize(QSize(20, 20));

        QPushButton* deleteEvent = new QPushButton();

        deleteEvent->setIcon(QIcon("deleteIcon.svg"));
        deleteEvent->setIconSize(QSize(20, 20));

        secondLine->addWidget(infoEvent);
        secondLine->addWidget(deleteEvent);
        secondLine->addWidget(time);

        QVBoxLayout* fullBox = new QVBoxLayout();

        fullBox->addLayout(firstLine);
        fullBox->addLayout(secondLine);

        fullBoxes->addLayout(fullBox);

        connect(infoEvent, &QPushButton::clicked, eventsList[i], &Event::handleShowEvent);
        connect(eventsList[i], &Event::showEvent, this, &MainWindow::handleShowInfoEventDialog);

        connect(deleteEvent, &QAbstractButton::clicked, eventsList[i], &Event::handleRemoveEvent);
    }

    evBoxes->setLayout(fullBoxes);
    ui->eventScrollArea->setWidget(evBoxes);
}

void MainWindow::printEmptyTodos() {
    // Show message on screen that no todos are available
    QVBoxLayout* firstLine = new QVBoxLayout();
    firstLine->setAlignment(Qt::AlignVCenter);

    QLabel* firstLabel = new QLabel("Nessun todo disponibile.");
    firstLabel->setStyleSheet("font-weight: bold;");
    firstLine->addWidget(firstLabel);

    QWidget* calBox = new QWidget(ui->todoScrollArea);
    calBox->setLayout(firstLine);
    ui->todoScrollArea->setWidget(calBox);
}

void MainWindow::printTodosList(QList <Todo*> todosList) {
    // Prepare the widget that will contain the list and its layout
    QWidget* tdBoxes = new QWidget(ui->todoScrollArea);

    QHBoxLayout* firstLine = new QHBoxLayout();
    QLabel* defaultText = new QLabel(QString("Todo per il " + ui->calendarWidget->selectedDate().toString()));
    defaultText->setStyleSheet("font-weight: bold;");
    firstLine->addWidget(defaultText);

    QVBoxLayout* fullBox = new QVBoxLayout();
    fullBox->setAlignment(Qt::AlignTop);
    fullBox->addLayout(firstLine);

    for(int i =0; i< todosList.length(); i++) {
        QHBoxLayout* theLine = new QHBoxLayout();
        QPushButton* theText = new QPushButton(QString(todosList[i]->summary()));

        QToolButton* deleteTodo = new QToolButton();

        deleteTodo->setIcon(QIcon("deleteIcon.svg"));
        deleteTodo->setIconSize(QSize(20, 20));
        QWidget* colourBox = new QWidget();

        colourBox->setStyleSheet(QString( "background-color: " + todosList[i]->colour() + ";" ));
        colourBox->setFixedSize(20, 20);

        // Make it rounded
        QPainterPath path;
        path.addRoundedRect(colourBox->rect(), 10, 10);
        QRegion mask = QRegion(path.toFillPolygon().toPolygon());
        colourBox->setMask(mask);

        theLine->addWidget(colourBox);
        theLine->addWidget(deleteTodo);
        theLine->addWidget(theText);
        fullBox->addLayout(theLine);

        connect(theText, &QPushButton::clicked, todosList[i], &Todo::handleShowTodo);
        connect(todosList[i], &Todo::showTodo, this, &MainWindow::handleShowInfoTodoDialog);

        connect(deleteTodo, &QAbstractButton::clicked, todosList[i], &Todo::handleRemoveTodo);
    }

    tdBoxes->setLayout(fullBox);
    ui->todoScrollArea->setWidget(tdBoxes);
}

void MainWindow::on_updateButton_clicked()
{
    emit refreshCalendars();
}

