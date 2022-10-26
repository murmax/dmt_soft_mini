#ifndef SQLLITECONTROL_H
#define SQLLITECONTROL_H

#include <QObject>

#include <QFile>

#include <QDebug>

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>

#include "common.h"

#define DATABASE "sqllite.db"


class SqlLiteControl : public QObject
{
    Q_OBJECT
public:
    explicit SqlLiteControl(QObject *parent = nullptr);

public slots:
    bool AddNewDev(QString dev_serial_number);

    bool AddNewFile(QString dev_serial_number, QString record_time, QString local_url, QString size);

    bool GetDevInfo(QString dev_serial_number, DB_DEV_INFO *dev_info);

    bool SetDevBatteryLvl(QString dev_serial_number, int battery_lvl);

    bool SetDevDownloadLvl(QString dev_serial_number, int download_lvl);

    bool SetOnlineTime(QString dev_serial_number, QString last_online_time);

    bool GetLocalFileList(QList<DB_FILE_INFO> *file_list);

signals:

private:
    bool ConnectToDataBase();

    bool InitTables();


private:
    QSqlDatabase db;
};

#endif // SQLLITECONTROL_H
