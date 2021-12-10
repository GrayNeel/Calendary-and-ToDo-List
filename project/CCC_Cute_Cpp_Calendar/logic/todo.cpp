#include "todo.h"

Todo::Todo(QObject *parent) : QObject(parent)
{

}

Todo::Todo(const QString &uid, const QString &filename, const QString &summary, const QDateTime &dueDateTime, const QString &colour) : _uid(uid),
    _filename(filename),
    _summary(summary),
    _dueDateTime(dueDateTime),
    _colour(colour) {
    if (uid.toInt()==-1)
      {
        //append a number generated from a random number generator if necessary
        _uid = QDateTime::currentDateTime().toString("yyyyMMdd-HHMM-00ss");
      }

      if (filename.isEmpty())
      {
        _filename = _uid + ".ics";
      }
}

const QString &Todo::uid() const
{
    return _uid;
}

void Todo::setUid(const QString &newUid)
{
    _uid = newUid;
}

const QString &Todo::filename() const
{
    return _filename;
}

void Todo::setFilename(const QString &newFilename)
{
    _filename = newFilename;
}

const QString &Todo::summary() const
{
    return _summary;
}

void Todo::setSummary(const QString &newSummary)
{
    _summary = newSummary;
}

const QDateTime &Todo::dueDateTime() const
{
    return _dueDateTime;
}

void Todo::setDueDateTime(const QDateTime &newDueDateTime)
{
    _dueDateTime = newDueDateTime;
}

const QString &Todo::etag() const
{
    return _etag;
}

void Todo::setEtag(const QString &newEtag)
{
    _etag = newEtag;
}

const QString &Todo::colour() const
{
    return _colour;
}

void Todo::setColour(const QString &newColour)
{
    _colour = newColour;
}

void Todo::showTodo(){
    QString italianDateFormat = "ddd dd/MM/yyyy";
    QString italianTimeFormat = "hh:mm";
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setText("Titolo:\t\t" + _summary + "\n"
                   "Data:\t" + _dueDateTime.date().toString(italianDateFormat) + "\n"
                   "Ora:\t" + _dueDateTime.time().toString(italianTimeFormat) + "\n");
    msgBox.exec();
}

void Todo::handleRemoveTodo(){
    // qDebug() << "Sono lo slot dentro event";
    emit removeTodo(this);
}


void Todo::handleEditTodo(){
    emit editTodo(this);
}
