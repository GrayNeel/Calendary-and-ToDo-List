#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    client = new Client();
    dialog = NULL;
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
    if(dialog == NULL)
        dialog = new Dialog();

    dialog->setModal("true");
    connect(dialog, &Dialog::eventAddCalendar, client, &Client::handleAddCalendar);
    connect(client, &Client::dialogErrorMessage, dialog, &Dialog::handleResponse);
    connect(client, &Client::closeDialog, this, &MainWindow::handleCloseDialog);
    //connect(client, &CLient::eventoConcluso, this, &MainWindow::deleteDialog);
    dialog->show();
}

void MainWindow::handleCloseDialog() {
    dialog->hide();
    disconnect(dialog, &Dialog::eventAddCalendar, client, &Client::handleAddCalendar);
    disconnect(client, &Client::dialogErrorMessage, dialog, &Dialog::handleResponse);
    disconnect(client, &Client::closeDialog, this, &MainWindow::handleCloseDialog);
    delete dialog;
    dialog = NULL;
}
