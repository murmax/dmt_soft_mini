#ifndef SQLLITECONTROL_H
#define SQLLITECONTROL_H

#include <QObject>

#include <QFile>

#include <QDebug>

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>

#include "common.h"
#include <QDateTime>

#define DATABASE "sqllite.db"

#define QDATETIME_FORMAT "yyyy-MM-dd hh:mm:ss"

struct DevInfo{
    QString dev_serial_number;
    DB_DEV_INFO devInfo;
};

class SqlLiteControl : public QObject
{
    Q_OBJECT
public:
    static SqlLiteControl* getSqlLiteControl(); //Singleton

public slots:
    bool AddNewDev(QString dev_serial_number);

    bool AddNewFile(QString dev_serial_number, QString record_time, QString local_url, QString size);

    bool GetDevInfo(QString dev_serial_number, DB_DEV_INFO *dev_info);

    bool SetDevBatteryLvl(QString dev_serial_number, int battery_lvl);

    bool SetDevDownloadLvl(QString dev_serial_number, int download_lvl);

    bool SetOnlineTime(QString dev_serial_number, QString last_online_time);

    bool GetLocalFileList(QList<DB_FILE_INFO> *file_list);

    bool loadDevs(QList<DevInfo> *output);

    bool loadFiles(QList<DB_FILE_INFO> *output);

    bool loadFilesWithFilters(QList<DB_FILE_INFO> *output,
                   int pageSize,
                   int pageNum,
                   QVariant filterMp3=QVariant(),
                   QVariant filterMp4=QVariant(),
                   QVariant filterJpg=QVariant(),
                   QVariant filterDateRecordStart=QVariant(),
                   QVariant filterDateRecordEnd=QVariant(),
                   QVariant filterDateDownloadStart=QVariant(),
                   QVariant filterDateDownloadEnd=QVariant(),
                   QVariant filterSerialNumber=QVariant());

    bool deleteFilesByUrl(QString url);


signals:

private:
    static SqlLiteControl* sqliteControlPtr;
    explicit SqlLiteControl(QObject *parent = nullptr);

    bool ConnectToDataBase();

    bool InitTables();


private:
    QSqlDatabase db;
    QString createCondition(QVariant filterMp3, QVariant filterMp4, QVariant filterJpg, QVariant filterDateRecordStart, QVariant filterDateRecordEnd, QVariant filterDateDownloadStart, QVariant filterDateDownloadEnd, QVariant filterSerialNumber);
};

#endif // SQLLITECONTROL_H
