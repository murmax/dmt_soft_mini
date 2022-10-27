#ifndef TABLEMODEL_H
#define TABLEMODEL_H
#include <qqml.h>
#include <QAbstractTableModel>
#include <QDateTime>
#include <QString>
#include <QDebug>
#include <QTimer>

class TableModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_PROPERTY(QString time READ time WRITE setTime NOTIFY timeChanged)
    //QML_ELEMENT

public:
    TableModel()
    {
        timer.setInterval(1000);
        timer.setSingleShot(false);
        connect(&timer,&QTimer::timeout, this, &TableModel::updateTime);
        timer.start();
    }
    int rowCount(const QModelIndex & = QModelIndex()) const override
    {
        return 200;
    }

    int columnCount(const QModelIndex & = QModelIndex()) const override
    {
        return 200;
    }

    QVariant data(const QModelIndex &index, int role) const override
    {
        switch (role) {
        case Qt::DisplayRole:
            return QString("%1, %2, %3").arg(index.column()).arg(index.row()).arg(time_s);
        default:
            break;
        }

        return QVariant();
    }

    QHash<int, QByteArray> roleNames() const override
    {
        return { {Qt::DisplayRole, "display"} };
    }

    QString time() const
    {
        return QDateTime::currentDateTime().toString("mm:ss");
    }
    void setTime(const QString &source)
    {
        beginResetModel();
        time_s=QDateTime::currentDateTime().toString("mm:ss");
        endResetModel();
    }
signals:
    void timeChanged();
private slots:
    void updateTime()
    {
        setTime("");
    }

private:
    QString time_s;
    QTimer timer;
};




#endif // TABLEMODEL_H
