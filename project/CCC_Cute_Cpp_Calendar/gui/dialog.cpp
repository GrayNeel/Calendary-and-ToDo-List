#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
}

Dialog::~Dialog()
{
    ui->usernameLine->clear();
    ui->passwordLine->clear();
    ui->urlLine->clear();
    delete ui;
}

void Dialog::on_loginButton_clicked()
{
    QString username = QString(ui->usernameLine->text());
    QString password = QString(ui->passwordLine->text());
    QString url = QString(ui->urlLine->text());
    emit eventAddCalendar(username, password, url);
}

void Dialog::handleResponse(QString errorMessage) {
    ui->errorMessageLabel->setText(errorMessage);
}

void Dialog::on_cancelButton_clicked()
{
    hide();
}

