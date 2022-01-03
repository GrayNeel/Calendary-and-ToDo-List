#ifndef TODODIALOG_H
#define TODODIALOG_H

#include <QDialog>
#include <QDateTime>
#include <QDate>
#include <QValidator>
#include "logic/calendar.h"

namespace Ui {
class TodoDialog;
}

class TodoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TodoDialog(QWidget *parent = nullptr);
    ~TodoDialog();

    Calendar *getCal() const;
    void setCal(Calendar *newCal);
    void setCalName(QString calname);

    Todo *getTodo() const;
    void setTodo(Todo *newTodo);

    void setFields();
    void disableFields(bool status);
    void setUpdating(bool value);

signals:
    void eventAddTodo(QString summary, QDateTime dueDateTime);
    void eventModifyTodo(QString oldUid, QString summary, QDateTime dueDateTime);
    void closeTodoDialog();

private slots:
    void on_confirmButton_clicked();

    void on_abortButton_clicked();

    void on_TodoDialog_rejected();

private:
    Ui::TodoDialog *ui;
    Calendar* cal;

    bool updating;
    Todo* todo;
    void handleTodoResponse(QString errorMessage);
};

#endif // TODODIALOG_H
