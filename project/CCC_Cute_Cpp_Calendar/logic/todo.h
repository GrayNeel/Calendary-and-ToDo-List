#ifndef TODO_H
#define TODO_H

#include <QObject>
#include <QDateTime>
#include <QMessageBox>

class Todo : public QObject
{
    Q_OBJECT
public:
    explicit Todo(QObject *parent = nullptr);
    Todo(const QString &uid, const QString &filename, const QString &summary, const QDateTime &dueDateTime, const QString &colour);

    const QString &uid() const;
    void setUid(const QString &newUid);
    const QString &filename() const;
    void setFilename(const QString &newFilename);
    const QString &summary() const;
    void setSummary(const QString &newSummary);
    const QDateTime &dueDateTime() const;
    void setDueDateTime(const QDateTime &newDueDateTime);
    const QString &etag() const;
    void setEtag(const QString &newEtag);
    const QString &colour() const;
    void setColour(const QString &newColour);

public slots:
    void handleShowTodo();
    void handleRemoveTodo();

signals:
    void showTodo(Todo* todo);
    void removeTodo(Todo* todo);

private:
    QString _uid;
    QString _filename;
    QString _summary;
    QDateTime _dueDateTime;
    QString _etag;

    QString _colour;
};

#endif // TODO_H
