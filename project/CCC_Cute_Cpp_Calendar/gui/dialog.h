#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QBuffer>
#include <QAuthenticator>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();

public slots:
    void handleAuth(QNetworkReply *reply, QAuthenticator *authenticator) const;
    void handleHTTPError() const;

private slots:
    void on_loginButton_clicked();
//    void handleHTTPError(void);

private:
    Ui::Dialog *ui;

    //TODO: Da spostare nel back-end
    QString username;
    QString password;
    QString url;
    QNetworkReply *reply;
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
};

#endif // DIALOG_H
