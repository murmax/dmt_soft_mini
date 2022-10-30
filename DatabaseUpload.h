#ifndef DATABASEUPLOAD_H
#define DATABASEUPLOAD_H

#include <QObject>
#include <QTimer>
#include <QtConcurrent>
#include "restinterface.h"
#include "SqlLiteControl.h"


class DatabaseUpload : public QObject
{
    Q_OBJECT
public:
    DatabaseUpload();

private slots:
    void fileUploadSuccessfully(QString url);
    void fileUploadFailed(QString url);

    void upload();

private:
    const int timerDelay = 1000; //мс между проверками базы

    QTimer uploadTimer;
    QMap<QString, QByteArray> devHashMap;
    QDateTime lastTimeUploaded=QDateTime::fromSecsSinceEpoch(0);

    QList<DevInfo> filterOnlyChangedDevHashes(QList<DevInfo> &devInfoList);
    bool checkDevHashChanged(DevInfo devInfo);
    QByteArray generateHash(DevInfo devInfo);
};

#endif // DATABASEUPLOAD_H
