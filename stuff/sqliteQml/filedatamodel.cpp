#include "filedatamodel.h"

FileDataModel::FileDataModel()
{
    //qmlRegisterType<FileDataModel>("com.ics.demo", 1, 0, "FileDataModel");
    //timer.setInterval(1000);
    //timer.setSingleShot(false);
    //connect(&timer,&QTimer::timeout, this, &FileDataModel::updateSelf);
    //timer.start();

    connect(this,&FileDataModel::modelNeedChange,this,&FileDataModel::updateSelf);
}

int FileDataModel::rowCount(const QModelIndex &parent) const
{
    return fileDataList.size();
}

int FileDataModel::columnCount(const QModelIndex &parent) const
{
    return 5;
}

QVariant FileDataModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(role == Qt::DisplayRole || role == Qt::EditRole ) {
        const auto& obj = fileDataList[index.row()];

        switch (index.column()) {
        case 0: return obj.dev_serial_number;
        case 1: return obj.download_time;
        case 2: return obj.record_time;
        case 3: return obj.file_size;
        case 4: return obj.local_url;
        }
    }
    return QVariant();
}

bool FileDataModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!index.isValid()) return false;
    if(role == Qt::EditRole){
        FileData& data = fileDataList[index.row()];
        switch (index.column()) {
        case 0: data.dev_serial_number = value.toString(); break;
        case 1: data.download_time = value.toDateTime(); break;
        case 2: data.record_time = value.toDateTime(); break;
        case 3: data.file_size = value.toInt(); break;
        case 4: data.local_url = value.toString(); break;
        }
    }
    return true;
}

bool FileDataModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginResetModel();
    for(;count > 0;count--) {
        fileDataList.removeAt(row);
    }
    endResetModel();
    return true;
}

QVariant FileDataModel::headerData(int section, Qt::Orientation orientation, int role) const
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
            return QVariant(QString::fromUtf8("download_time"));
        case 2:
            return QVariant(QString::fromUtf8("record_time"));
        case 3:
            return QVariant(QString::fromUtf8("file_size"));
        case 4:
            return QVariant(QString::fromUtf8("local_url"));
        default:
            return QVariant();
        }
    }else if(orientation == Qt::Vertical){
        return section;
    }
    return QVariant();
}


void FileDataModel::appendItem(FileData file)
{
    fileDataList.append(std::move(file));
}

QString FileDataModel::toJSON()
{
    QJsonArray objectsArray;
    for (FileData data : fileDataList) {
        QJsonObject object_data;
        object_data.insert("dev_serial_number", QJsonValue::fromVariant(data.dev_serial_number));
        object_data.insert("download_time", QJsonValue::fromVariant(data.download_time));
        object_data.insert("record_time", QJsonValue::fromVariant(data.record_time));
        object_data.insert("file_size", QJsonValue::fromVariant(data.file_size));
        object_data.insert("local_url", QJsonValue::fromVariant(data.local_url));
        objectsArray.push_back(object_data);
    }
    QJsonDocument doc(objectsArray);
    QString jsonString = doc.toJson(QJsonDocument::Indented);
    return jsonString;
}

void FileDataModel::setPageSize(int size)
{
  qDebug() << __func__ << " size=" << size;
  this->pageSize = size;
}

void FileDataModel::setPageNum(int num)
{
  qDebug() << __func__ << " num=" << num;
  this->pageNum = num;
}

int FileDataModel::getPageSize()
{
  return pageSize;
}

int FileDataModel::getPageNum()
{
  return pageNum;
}

void FileDataModel::updateSelf()
{
    qDebug() << "file updateSelf";
    auto sqliteWorker_ptr =  SqliteWorker::getSqliteWorker();
    auto files = sqliteWorker_ptr->loadFiles(pageSize,
                                             pageNum,
                                             filterMp3,
                                             filterMp4,
                                             filterJpg,
                                             filterDateRecordStart,
                                             filterDateRecordEnd,
                                             filterDateDownloadStart,
                                             filterDateDownloadEnd,
                                             filterSerialNumber);
    beginResetModel();
    removeRows(0,rowCount(QModelIndex()),QModelIndex()); //очитска модели перед каждой синхронизацией
    for (const auto &row : qAsConst(files))
    {
        appendItem(row);
    }
    endResetModel();
}

