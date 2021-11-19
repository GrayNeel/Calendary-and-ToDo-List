#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //dialog = new Dialog();
    client = new Client();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete dialog;
    delete client;
}


void MainWindow::on_actionApri_calendario_triggered()
{
    dialog = new Dialog();
    dialog->setModal("true");
    connect(dialog, &Dialog::eventAddCalendar, client, &Client::handleAddCalendar);
    //connect(client, &CLient::eventoConcluso, this, &MainWindow::deleteDialog);
    dialog->show();
}
