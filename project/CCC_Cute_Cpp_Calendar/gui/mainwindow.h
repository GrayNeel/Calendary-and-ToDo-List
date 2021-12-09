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
    void on_calendarWidget_selectionChanged();

    void handleCloseDialog();
    void handleAddCalendarFinished(Calendar* cal);

    void handleRemoveCalendarBox(Calendar* cal);
    void eventShowEventDialog(Calendar* cal);
    void handleAddEventFinished();
    void handleAddEventError();
    void handleAddEventWithoutError();
    void handlePrintEvent(QList<Event*> eventList);


private:
    Ui::MainWindow *ui;

    Dialog* dialog;
    EventDialog* eventDialog;

    Client* client;

    QWidget* _calBoxes;
    QVBoxLayout* _calBoxesLayout;
};
#endif // MAINWINDOW_H
