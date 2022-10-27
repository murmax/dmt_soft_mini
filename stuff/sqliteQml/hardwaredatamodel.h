#ifndef USERDATAMODEL_H
#define USERDATAMODEL_H

#include <QAbstractTableModel>
#include <QDateTime>
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QTimer>
#include <QtQml/qqml.h>
#include "sqliteworker.h"
//структура для хранения данных из бд

//Класс мождели файлов, хранит данные о файлах их бд в контейнере hardwareDataList
class HardwareDataModel : public QAbstractTableModel
{
    Q_OBJECT
    QML_ELEMENT
public:
    explicit HardwareDataModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    bool removeRows(int row, int count, const QModelIndex &parent);
    Q_INVOKABLE QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    //Функция добавления нового элемента в модель (В конец внутреннего контейнера). Используется при заполнении из бд.
    void appendItem(HardwareData hard);
    //Функция конвертации данных из модели в json
    QString toJSON();

public slots:
    Q_INVOKABLE void updateSelf();

private:
    //Контейнер для хранения объектов из бд
    QList<HardwareData> hardwareDataList;
    //Таймер обновления
    //QTimer timer;
};
#endif // USERDATAMODEL_H
