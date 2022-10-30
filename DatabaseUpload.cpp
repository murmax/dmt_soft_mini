#include "DatabaseUpload.h"
#include "qtconcurrentfilter.h"

#include <QUrl>


DatabaseUpload::DatabaseUpload()
{
    uploadTimer.setInterval(5000);
    uploadTimer.setSingleShot(false);
    connect(&uploadTimer,&QTimer::timeout,this,&DatabaseUpload::upload);
    uploadTimer.start();
    connect(RestInterface::getRestInterface(),&RestInterface::fileSentSuccess,this,&DatabaseUpload::fileUploadSuccessfully);
    connect(RestInterface::getRestInterface(),&RestInterface::fileSentFail,this,&DatabaseUpload::fileUploadFailed);
}

void DatabaseUpload::fileUploadSuccessfully(QString url)
{
    QUrl urlFile(url);
    QFile::remove(urlFile.toLocalFile());
    SqlLiteControl::getSqlLiteControl()->deleteFilesByUrl(url);
}

void DatabaseUpload::fileUploadFailed(QString url)
{
    qDebug()<<"fileUploadFail on URL:"<<url;
}

void DatabaseUpload::upload()
{
    RestInterface* rest = RestInterface::getRestInterface();
    SqlLiteControl* sqlite = SqlLiteControl::getSqlLiteControl();
    QList<DevInfo> devs;
    sqlite->loadDevs(&devs);
    QList<DevInfo> filteredDevs = filterOnlyChangedDevHashes(devs);
    for (DevInfo devInfo : qAsConst(filteredDevs))
    {
        devHashMap.insert(devInfo.dev_serial_number,generateHash(devInfo));
        rest->sendPutDev(&devInfo);
    }

    QList<DB_FILE_INFO> files;
    sqlite->loadFiles(&files);
    for (auto file : qAsConst(files))
    {
        if (lastTimeUploaded.toMSecsSinceEpoch()+uploadTimer.interval()
                <
                QDateTime::fromString(file.download_time,QDATETIME_FORMAT).toMSecsSinceEpoch()
            )
        {
            rest->sendPostFile(&file);
        }
    }
    lastTimeUploaded = QDateTime::currentDateTime();
}

QList<DevInfo> DatabaseUpload::filterOnlyChangedDevHashes(QList<DevInfo> &devInfoList)
{
    auto keepFunc =  [this](DevInfo info) -> bool{
        return checkDevHashChanged(info);
    };
    QList<DevInfo> filtered = QtConcurrent::blockingFiltered(devInfoList,keepFunc);
    return filtered;
}

bool DatabaseUpload::checkDevHashChanged(DevInfo devInfo)
{
    if (!devHashMap.contains(devInfo.dev_serial_number))
        return true;
    QByteArray oldValue = devHashMap.value(devInfo.dev_serial_number);
    QByteArray newValue =  generateHash(devInfo);
    return oldValue != newValue;
}

QByteArray DatabaseUpload::generateHash(DevInfo devInfo)
//Именно тут задается, на изменение каких полей будет реагировать программа. Если нужно больше полей - нужно их добавть к хешу
{
    return QCryptographicHash::hash(
                (
                    QString::number(devInfo.devInfo.battery_lvl)
                    +"_"+
                    QString::number(devInfo.devInfo.download_lvl)
                ).toUtf8(),
                QCryptographicHash::Md5
                );
}
