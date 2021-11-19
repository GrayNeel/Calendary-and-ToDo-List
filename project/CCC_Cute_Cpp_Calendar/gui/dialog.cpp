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
    delete ui;
}

void Dialog::on_loginButton_clicked()
{
    username = QString(ui->usernameLine->text());
    password = QString(ui->passwordLine->text());
    url = QString(ui->urlLine->text());

    //    QAuthenticator q = QAuthenticator();
    //    q.setUser(username);
    //    q.setPassword(password);

    QBuffer* buffer = new QBuffer();
    buffer->open(QIODevice::ReadWrite);
    buffer->write("");

    QNetworkRequest request;
    request.setAttribute(QNetworkRequest::Http2AllowedAttribute, false);
    request.setUrl(QUrl(url));
    request.setRawHeader("Host", "127.0.0.1:8080");
    request.setRawHeader("User-Agent", "Cute_Cpp_Calendar_Client");
    request.setRawHeader("Depth", "0");
    request.setRawHeader("Prefer", "return-minimal");
    request.setRawHeader("Content-Type", "text/xml; charset=utf-8");
    request.setRawHeader("Content-Length", 0);


    reply = manager->sendCustomRequest(request, QByteArray("GET"), buffer);
    connect(reply, SIGNAL(finished()), this, SLOT(handleHTTPError()));
    connect(manager, &QNetworkAccessManager::authenticationRequired, this, &Dialog::handleAuth);
    hide();
}

void Dialog::handleAuth(QNetworkReply *reply, QAuthenticator *q) const{
    q->setUser(username);
    q->setPassword(password);
}

void Dialog::handleHTTPError() const {
    //if(reply->rawHeader("Server"))
//    qDebug() << QString(reply->rawHeader("Server"));
}
