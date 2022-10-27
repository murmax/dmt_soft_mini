#include "hardwaredatamodel.h"

HardwareDataModel::HardwareDataModel(QObject* parent)
{
    //qmlRegisterType<HardwareDataModel>("com.ics.demo", 1, 0, "HardwareDataModel");
    //timer.setInterval(1000);
    //timer.setSingleShot(true);
    //connect(&timer,&QTimer::timeout, this, &HardwareDataModel::updateSelf);
    //timer.start();
}

int HardwareDataModel::rowCount(const QModelIndex &parent) const
{
    return hardwareDataList.size();
}

int HardwareDataModel::columnCount(const QModelIndex &parent) const
{
    return 5;
}

QVariant HardwareDataModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(role == Qt::DisplayRole || role == Qt::EditRole ) {
        auto const & obj = hardwareDataList.at(index.row());

        switch (index.column()) {
        case 0: return obj.dev_serial_number;
        case 1: return obj.registration_time;
        case 2: return obj.last_online_time;
        case 3: return obj.battery_lvl;
        case 4: return obj.download_lvl;
        }
    }
    return QVariant();
}

bool HardwareDataModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!index.isValid()) return false;
    if(role == Qt::EditRole){
        assert(index.row() < hardwareDataList.size());
        auto& data = hardwareDataList[index.row()];
        switch (index.column()) {
        case 0: data.dev_serial_number = value.toString(); break;
        case 1: data.registration_time = value.toDateTime(); break;
        case 2: data.last_online_time = value.toDateTime(); break;
        case 3: data.battery_lvl = value.toInt(); break;
        case 4: data.download_lvl = value.toInt(); break;
        }
    }
    return true;
}

bool HardwareDataModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginResetModel();
    for(;count > 0;count--) {
        hardwareDataList.removeAt(row);
    }
    endResetModel();
    return true;
}

QVariant HardwareDataModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();
    if (orientation == Qt::Horizontal)
    {
        switch (section)
        {
        case 0:
            return QVariant(QString::fromUtf8("dev_serial_number"));
        case 1:
            return QVariant(QString::fromUtf8("registration_time"));
        case 2:
            return QVariant(QString::fromUtf8("last_online_time"));
        case 3:
            return QVariant(QString::fromUtf8("battery_lvl"));
        case 4:
            return QVariant(QString::fromUtf8("download_lvl"));
        default:
            return QVariant();
        }
    }else if(orientation == Qt::Vertical){
        return section;
    }
    return QVariant();
}

void HardwareDataModel::appendItem(HardwareData hard) //ВРЕМЕННО
{
    hardwareDataList.append(hard);
}

QString HardwareDataModel::toJSON()
{
    QJsonArray objectsArray;
    for (HardwareData data : hardwareDataList) {
        QJsonObject object_data;
        object_data.insert("dev_serial_number", QJsonValue::fromVariant(data.dev_serial_number));
        object_data.insert("registration_time", QJsonValue::fromVariant(data.registration_time));
        object_data.insert("last_online_time", QJsonValue::fromVariant(data.last_online_time));
        object_data.insert("battery_lvl", QJsonValue::fromVariant(data.battery_lvl));
        object_data.insert("download_lvl", QJsonValue::fromVariant(data.download_lvl));
        objectsArray.push_back(object_data);
    }
    QJsonDocument doc(objectsArray);
    QString jsonString = doc.toJson(QJsonDocument::Indented);
    return jsonString;
}

void HardwareDataModel::updateSelf()
{
    qDebug() << "hardware updateSelf";
    auto sqliteWorker_ptr =  SqliteWorker::getSqliteWorker();
    auto hardwares = sqliteWorker_ptr->loadHardware();
    qDebug() << "hardwares.length()" << hardwares.length();
    beginResetModel();
    removeRows(0,rowCount(QModelIndex()),QModelIndex()); //очитска модели перед каждой синхронизацией
    for (const auto &row : qAsConst(hardwares))
    {
        appendItem(row);
    }
    endResetModel();
}

