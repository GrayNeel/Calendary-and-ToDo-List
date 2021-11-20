#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QGroupBox>

#include "dialog.h"

#include "logic/client.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    //void setClient(Client *client);

private slots:
    void on_actionApri_calendario_triggered();
    void handleCloseDialog(Calendar* cal);

    void on_scrollArea_customContextMenuRequested(const QPoint &pos);

private:
    Ui::MainWindow *ui;
    Dialog* dialog;
    Client* client;

    QWidget* _test;
    QHBoxLayout* layout;
};
#endif // MAINWINDOW_H
