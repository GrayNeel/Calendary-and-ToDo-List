#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QGroupBox>

#include "dialog.h"
#include "eventdialog.h"

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

    void on_calendarWidget_clicked(const QDate &date);

    void on_pushButton_clicked();
    void handleCloseEventDialog(Event* event);
    void handleRemoveCalendarBox(Calendar* cal);

    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;
    Dialog* dialog;
    EventDialog* eventDialog;
    Client* client;

    QMap<QString,QVBoxLayout*> _boxesMap;

    QWidget* _calBoxes;
    QVBoxLayout* _calBoxesLayout;
};
#endif // MAINWINDOW_H
