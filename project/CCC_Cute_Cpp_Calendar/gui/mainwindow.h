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

private slots:
    void on_actionApri_calendario_triggered();
    void on_calendarWidget_clicked(const QDate &date);

    /* Custom Slots */
    void handleCloseDialog();

    void eventShowEventDialog(Calendar* cal);
    void handleAddEventFinished();
    void handleAddEventError();
    void handleAddEventWithoutError();
    void handleShowModifyEventDialog(Event *event);

    void handleUpdateMainWindowWidgets();

    void handleCloseModifyEventDialog();
private:
    Ui::MainWindow *ui;

    Dialog* dialog;
    EventDialog* eventDialog;

    Client* client;

    /* Private functions for GUI updates */
    void printEmptyCalendars();
    void printCalendars();
    void printEmptyEvents();
    void printEventsList(QList <Event*> eventsList);
    void printEmptyTodos();

};
#endif // MAINWINDOW_H
