#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QGroupBox>

#include "dialog.h"
#include "eventdialog.h"
#include "tododialog.h"

#include "logic/client.h"
#include "logic/worker.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void refreshCalendars();
    void operateThread();

private slots:
    void on_actionApri_calendario_triggered();
    void on_calendarWidget_clicked(const QDate &date);

    /* Custom Slots */
    void handleCloseDialog();

    void eventShowEventDialog(Calendar* cal);
    void handleAddEventFinished();
    void handleAddEventError();
    void handleAddEventWithoutError();
    void handleShowInfoEventDialog(Event *event);
    void handleShowModifyEventDialog();

    void handleUpdateMainWindowWidgets();

    void handleCloseModifyEventDialog();
    void handleModifyEventWithoutError();
    void handleModifyEventError();

    void handleCloseModifyTodoDialog();
    void handleModifyTodoWithoutError();
    void handleModifyTodoError();

    void eventShowTodoDialog(Calendar *cal);
    void handleAddTodoFinished();
    void handleAddTodoError();
    void handleAddTodoWithoutError();
    void handleShowInfoTodoDialog(Todo* todo);
    void handleShowModifyTodoDialog();

    void on_updateButton_clicked();
    void handleAutomaticRefresh();

private:
    Ui::MainWindow *ui;

    Dialog* dialog;
    EventDialog* eventDialog;
    TodoDialog* todoDialog;

    Client* client;

    /* Private functions for GUI updates */
    void printEmptyCalendars();
    void printCalendars();
    void printEmptyEvents();
    void printEventsList(QList <Event*> eventsList);
    void printTodosList(QList<Todo*> todosList);
    void printEmptyTodos();

    QThread workerThread;

};
#endif // MAINWINDOW_H
