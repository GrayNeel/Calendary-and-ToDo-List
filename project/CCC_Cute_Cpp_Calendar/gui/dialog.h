#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QBuffer>
#include <QAuthenticator>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

#include "logic/client.h"

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();
    void setClient(Client *client);

public slots:
    void handleResponse(QString errorMessage);

private slots:
    void on_loginButton_clicked();
//    void handleHTTPError(void);

    void on_cancelButton_clicked();

signals:
    void eventAddCalendar(QString username, QString password, QString url);

private:
    Ui::Dialog *ui;

    //TODO: Da spostare nel back-end

};

#endif // DIALOG_H
