#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    client = new Client();
    dialog = NULL;
    _test = new QWidget();
    layout = new QHBoxLayout();
}

MainWindow::~MainWindow()
{
    delete ui;
    if(dialog != NULL)
        delete dialog;
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

    // Create the widget to show calendar on main window
    QLabel* name = new QLabel(cal->displayName());
    layout->addWidget(name);
    _test->setLayout(layout);
    ui->scrollArea->setWidget(_test);
}

void MainWindow::on_scrollArea_customContextMenuRequested(const QPoint &pos)
{

}

