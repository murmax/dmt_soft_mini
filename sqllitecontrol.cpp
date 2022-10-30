#include "sqllitecontrol.h"

SqlLiteControl* SqlLiteControl::sqliteControlPtr=nullptr;

SqlLiteControl::SqlLiteControl(QObject *parent)
    : QObject{parent}
{
    ConnectToDataBase();
    InitTables();
}

SqlLiteControl *SqlLiteControl::getSqlLiteControl()
{
    if (!sqliteControlPtr)
    {
        sqliteControlPtr = new SqlLiteControl();
    }
    return sqliteControlPtr;
}

bool SqlLiteControl::AddNewDev(QString serial_number)
{
    bool error_flag;
    QSqlQuery query = QSqlQuery(db);

    error_flag = query.exec(
                QString (
                    "INSERT INTO  devices "
                    "("
                "dev_serial_number"
            ")"
            "VALUES"
            "("
                "%1"
            ");"
        ).arg(serial_number)
    );

    if(!error_flag) {
        qDebug() << query.lastError().text();
    }


}

bool SqlLiteControl::AddNewFile(QString dev_serial_number, QString record_time, QString local_url, QString file_size)
{
    bool error_flag;
    QSqlQuery query = QSqlQuery(db);

    query.prepare (
        "INSERT INTO  mediafiles "
        "("
            "dev_serial_number,"
            "record_time,"
            "local_url,"
            "file_size"
        ")"
        "VALUES"
        "("
            ":dev_serial_number,"
            ":record_time,"
            ":local_url,"
            ":file_size"
        ");"
    );

    query.bindValue(":dev_serial_number", dev_serial_number);
    query.bindValue(":record_time", record_time);
    query.bindValue(":local_url", local_url);
    query.bindValue(":file_size", file_size);

    error_flag = query.exec();

    if(!error_flag) {
        qDebug() << query.lastError().text();
    }
}

bool SqlLiteControl::GetDevInfo(QString dev_serial_number, DB_DEV_INFO *dev_info)
{
    bool error_flag;
    QSqlQuery query = QSqlQuery(db);

    query.prepare( "SELECT * FROM devices WHERE dev_serial_number = :dev_serial_number" );

    query.bindValue( ":dev_serial_number",  dev_serial_number);

    error_flag = query.exec();

    if(error_flag) {
        if (query.next()) {
            dev_info->username = query.value(2).toString();
            dev_info->registration_time = query.value(3).toString();
            dev_info->last_online_time = query.value(3).toString();
            dev_info->battery_lvl = query.value(4).toInt();
            dev_info->download_lvl = query.value(5).toInt();
            return true;
        }
        else {
            return false;
        }
    }
    else {
        qDebug() << query.lastError().text();
        return false;
    }

}

bool SqlLiteControl::SetDevBatteryLvl(QString dev_serial_number, int battery_lvl)
{
    bool error_flag;
    QSqlQuery query = QSqlQuery(db);

    query.prepare (
        "UPDATE devices SET battery_lvl = :battery_lvl"
        " WHERE dev_serial_number = :dev_serial_number"
    );

    query.bindValue(":battery_lvl", battery_lvl);
    query.bindValue(":dev_serial_number", dev_serial_number);

    error_flag = query.exec();

    if(!error_flag) {
        qDebug() << query.lastError().text();
    }

}

bool SqlLiteControl::SetDevDownloadLvl(QString dev_serial_number, int download_lvl)
{
    bool error_flag;
    QSqlQuery query = QSqlQuery(db);

    query.prepare (
        "UPDATE devices SET download_lvl = :download_lvl"
        " WHERE dev_serial_number = :dev_serial_number"
    );

    query.bindValue(":download_lvl", download_lvl);
    query.bindValue(":dev_serial_number", dev_serial_number);

    error_flag = query.exec();

    if(!error_flag) {
        qDebug() << query.lastError().text();
    }
}

bool SqlLiteControl::SetOnlineTime(QString dev_serial_number, QString last_online_time)
{
    bool error_flag;
    QSqlQuery query = QSqlQuery(db);

    query.prepare (
        "UPDATE devices SET last_online_time = :last_online_time"
        " WHERE dev_serial_number = :dev_serial_number"
    );

    query.bindValue(":last_online_time", last_online_time);
    query.bindValue(":dev_serial_number", dev_serial_number);

    error_flag = query.exec();

    if(!error_flag) {
        qDebug() << query.lastError().text();
    }
}

bool SqlLiteControl::GetLocalFileList(QList<DB_FILE_INFO> *file_list)
{
    bool error_flag;
    QSqlQuery query = QSqlQuery(db);

    query.prepare( "SELECT * FROM mediafiles" );

    error_flag = query.exec();

    if(error_flag) {
        while(query.next()) {
            DB_FILE_INFO file_info;
            file_info.dev_serial_number = query.value(1).toString();
            file_info.download_time = query.value(2).toString();
            file_info.record_time = query.value(3).toString();
            file_info.local_url = query.value(5).toInt();
            file_info.size = query.value(4).toInt();
            file_list->append(file_info);
        }
    }
    else {
        qDebug() << query.lastError().text();
        return false;
    }
}

bool SqlLiteControl::loadDevs(QList<DevInfo> *output)
{
    QSqlQuery query(db);
    if (!query.exec("SELECT * FROM devices")) {
      qDebug() << "Ошибка" << query.lastError() << "в запросе:" << query.lastQuery();
      return false;
    }
    QSqlRecord rec = query.record();
    while (query.next()) {
        DevInfo file_info = {
            query.value(rec.indexOf("dev_serial_number")).toString(),
            {
                query.value(rec.indexOf("username")).toString(),
                query.value(rec.indexOf("registration_time")).toString(),
                query.value(rec.indexOf("last_online_time")).toString(),
                query.value(rec.indexOf("battery_lvl")).toInt(),
                query.value(rec.indexOf("download_lvl")).toInt()
            }
        };
        output->append(file_info);
    }
    return true;
}

bool SqlLiteControl::loadFiles(QList<DB_FILE_INFO> *output)
{
    QSqlQuery query(db);
    QString queryText = "SELECT * FROM mediafiles";

    if (!query.exec(queryText)) {
        qDebug() << "Ошибка" << query.lastError() << "в запросе:" << query.lastQuery();
        return false;
    }
    QSqlRecord rec = query.record();
    while (query.next()) {
        DB_FILE_INFO dev_info = {
            query.value(rec.indexOf("dev_serial_number")).toString(),
            query.value(rec.indexOf("download_time")).toString(),
            query.value(rec.indexOf("record_time")).toString(),
            query.value(rec.indexOf("file_size")).toString(),
            query.value(rec.indexOf("local_url")).toString()
        };
        output->append(dev_info);
     }
    return true;
}

bool SqlLiteControl::loadFilesWithFilters(QList<DB_FILE_INFO> *output, int pageSize, int pageNum, QVariant filterMp3, QVariant filterMp4, QVariant filterJpg, QVariant filterDateRecordStart, QVariant filterDateRecordEnd, QVariant filterDateDownloadStart, QVariant filterDateDownloadEnd, QVariant filterSerialNumber)
{
    QSqlQuery query(db);
    QString condition = createCondition
            (
                filterMp3,
                filterMp4,
                filterJpg,
                filterDateRecordStart,
                filterDateRecordEnd,
                filterDateDownloadStart,
                filterDateDownloadEnd,
                filterSerialNumber
                );
    QString queryText = "SELECT * FROM mediafiles"
            + condition
            + " LIMIT "
            + QString::number(pageSize)
            + " OFFSET "
            + QString::number(pageNum*pageSize);

    if (!query.exec(queryText)) {
        qDebug() << "Ошибка" << query.lastError() << "в запросе:" << query.lastQuery();
        return false;
    }
    QSqlRecord rec = query.record();
    while (query.next()) {
        DB_FILE_INFO dev_info = {
            query.value(rec.indexOf("dev_serial_number")).toString(),
            query.value(rec.indexOf("download_time")).toString(),
            query.value(rec.indexOf("record_time")).toString(),
            query.value(rec.indexOf("file_size")).toString(),
            query.value(rec.indexOf("local_url")).toString()
        };
        output->append(dev_info);
    }
    return true;
}

bool SqlLiteControl::deleteFilesByUrl(QString url)
{
    QSqlQuery query(db);
    QString queryText = QString("DELETE FROM mediafiles WHERE local_url='%1'").arg(url);

    if (!query.exec(queryText)) {
        qDebug() << "Ошибка" << query.lastError() << "в запросе:" << query.lastQuery();
        return false;
    }
    return true;
}

QString SqlLiteControl::createCondition(QVariant filterMp3,
                                        QVariant filterMp4,
                                        QVariant filterJpg,
                                        QVariant filterDateRecordStart,
                                        QVariant filterDateRecordEnd,
                                        QVariant filterDateDownloadStart,
                                        QVariant filterDateDownloadEnd,
                                        QVariant filterSerialNumber)
{
    QString result = "";
    if(filterMp3.canConvert(QMetaType::Bool)){
        if(filterMp3.toBool()){
            result.append("local_url LIKE '%.mp3'");
        }
    }
    if(filterMp4.canConvert(QMetaType::Bool)){
        if(filterMp4.toBool()){
            if(result != ""){
                result.append(" AND ");
            }
            result.append("local_url LIKE '%.mp4'");
        }
    }
    if(filterJpg.canConvert(QMetaType::Bool)){
        if(filterJpg.toBool()){
            if(result != ""){
                result.append(" AND ");
            }
            result.append("local_url LIKE '%jpg'");
        }
    }
    QDateTime buffDt;
    buffDt = filterDateRecordStart.toDateTime();
    if(buffDt.isValid()){
        if(result != ""){
            result.append(" AND ");
        }
        result.append("strftime('%s',record_time) >= strftime('%s','" + buffDt.toString("yyyy-MM-dd  hh:mm:ss") + "')");
    }
    buffDt = filterDateRecordEnd.toDateTime();
    if(buffDt.isValid()){
        if(result != ""){
            result.append(" AND ");
        }
        result.append("strftime('%s',record_time) <= strftime('%s','" + buffDt.toString("yyyy-MM-dd  hh:mm:ss") + "')");
    }
    buffDt = filterDateDownloadStart.toDateTime();
    if(buffDt.isValid()){
        if(result != ""){
            result.append(" AND ");
        }
        result.append("strftime('%s',download_time) >= strftime('%s','" + buffDt.toString("yyyy-MM-dd  hh:mm:ss") + "')");
    }
    buffDt = filterDateDownloadEnd.toDateTime();
    if(buffDt.isValid()){
        if(result != ""){
            result.append(" AND ");
        }
        result.append("strftime('%s',download_time) <= strftime('%s','" + buffDt.toString("yyyy-MM-dd  hh:mm:ss") + "')");
    }
    if(filterSerialNumber.toString() != ""){
        if(result != ""){
            result.append(" AND ");
        }
        result.append("dev_serial_number = " + filterSerialNumber.toString());
    }
    if(result != ""){
        result.prepend(" WHERE ");
    }
    return result;
}


bool SqlLiteControl::ConnectToDataBase()
{
    QFile sqllitefile;
    sqllitefile.setFileName(DATABASE);

    if (!sqllitefile.exists())
    {
        if(sqllitefile.open(QIODevice::WriteOnly))
        {
            qDebug() << "Файл БД создан";
        }
        else
        {
            qDebug() << "Файл БД не создан";
        }
        sqllitefile.close();
    }
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(DATABASE);
    if(db.open()) {
        return true;
    }
    else {
        qDebug() << db.lastError().text();
        return false;
    }

}

bool SqlLiteControl::InitTables()
{
    bool error_flag;
    QSqlQuery query = QSqlQuery(db);

    error_flag = query.exec(
        "CREATE TABLE devices"
        "("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "dev_serial_number varchar(255) NOT NULL,"
            "username varchar(255),"
            "registration_time DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,"
            "last_online_time DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,"
            "battery_lvl INTEGER NOT NULL DEFAULT 0,"
            "download_lvl INTEGER NOT NULL DEFAULT 0"
        ");"
    );

    if(!error_flag) {
        qDebug() << query.lastError().text();
    }

    error_flag = query.exec(
        "CREATE TABLE mediafiles"
        "("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "dev_serial_number VARCHAR(255),"
            "download_time DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,"
            "record_time DATETIME DEFAULT CURRENT_TIMESTAMP,"
            "file_size VARCHAR(255) NOT NULL DEFAULT 0,"
            "local_url VARCHAR(255) NOT NULL,"
            "network_url VARCHAR(255) NOT NULL DEFAULT none"
        ");"
    );

    if(!error_flag) {
        qDebug() << query.lastError().text();
    }

}

