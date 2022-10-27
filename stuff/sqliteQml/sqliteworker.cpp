#include "sqliteworker.h"

SqliteWorker* SqliteWorker::sqliteworker_ptr=nullptr;

SqliteWorker::SqliteWorker()
{
  if(connectToDataBase()){
    //this->setPageNum(0);
    //this->setPageSize(10);
    //databaseResyncTimer = new QTimer();
    //QObject::connect(databaseResyncTimer,SIGNAL(timeout()), this, SLOT(syncWithDB()));
    //databaseResyncTimer->setSingleShot(false);
    //databaseResyncTimer->start(10000);
    //qDebug().noquote()<<"Resync"<<hardwareModel.toJSON();
  }
}

SqliteWorker *SqliteWorker::getSqliteWorker()
{
  if (!sqliteworker_ptr)
  {
    sqliteworker_ptr = new SqliteWorker();
  }
  return sqliteworker_ptr;
}

bool SqliteWorker::connectToDataBase()
{
  dbase = QSqlDatabase::addDatabase("QSQLITE");
  dbase.setDatabaseName("sqllite.db");
  if (!dbase.open()) {
    qDebug() << "Что-то пошло не так!"<<dbase.lastError().text();
    return false;
  }
  return true;
}

void SqliteWorker::syncWithDB()
{
  //hardwareModel.removeRows(0,hardwareModel.rowCount(QModelIndex()),QModelIndex()); //очитска модели перед каждой синхронизацией
  //fileModel.removeRows(0,fileModel.rowCount(QModelIndex()),QModelIndex()); //очитска модели перед каждой синхронизацией
  //QSqlQuery a_query(dbase);
  //loadHardware();
  //qDebug().noquote() << hardwareModel.toJSON();
}
QList<HardwareData> SqliteWorker::loadHardware()
{
  QSqlQuery query(dbase);
  QList<HardwareData> result;
  if (!query.exec("SELECT * FROM devices")) {
    qDebug() << "Ошибка" << query.lastError();
    return result;
  }
  QSqlRecord rec = query.record();
  while (query.next()) {
    QString dev_serial_number = query.value(rec.indexOf("dev_serial_number")).toString();
    QDateTime registration_time = query.value(rec.indexOf("registration_time")).toDateTime();
    QDateTime last_online_time = query.value(rec.indexOf("last_online_time")).toDateTime();
    int battery_lvl = query.value(rec.indexOf("battery_lvl")).toInt();
    int download_lvl = query.value(rec.indexOf("download_lvl")).toInt();
    result.append(HardwareData{dev_serial_number, registration_time, last_online_time, battery_lvl, download_lvl});
  }
  return result;
}

QList<FileData> SqliteWorker::loadFiles(int pageSize,
                                        int pageNum,
                                        QVariant filterMp3,
                                        QVariant filterMp4,
                                        QVariant filterJpg,
                                        QVariant filterDateRecordStart,
                                        QVariant filterDateRecordEnd,
                                        QVariant filterDateDownloadStart,
                                        QVariant filterDateDownloadEnd,
                                        QVariant filterSerialNumber)
{
  QSqlQuery query(dbase);
  QList<FileData> result;
  QString condition = createCondition(filterMp3,
                                      filterMp4,
                                      filterJpg,
                                      filterDateRecordStart,
                                      filterDateRecordEnd,
                                      filterDateDownloadStart,
                                      filterDateDownloadEnd,
                                      filterSerialNumber);
  QString queryText = "SELECT * FROM mediafiles"
      + condition
      + " LIMIT "
      + QString::number(pageSize)
      + " OFFSET "
      + QString::number(pageNum*pageSize);
  //qDebug() << "Попытка выполнить:";
  //qDebug() << queryText;

  if (!query.exec(queryText)) {
    qDebug() << "Ошибка" << query.lastError();
    return result;
  }
  QSqlRecord rec = query.record();
  while (query.next()) {
    QString dev_serial_number = query.value(rec.indexOf("dev_serial_number")).toString();
    QDateTime download_time = query.value(rec.indexOf("download_time")).toDateTime();
    QDateTime record_time = query.value(rec.indexOf("record_time")).toDateTime();
    QString file_size = query.value(rec.indexOf("file_size")).toString();
    QString local_url = query.value(rec.indexOf("local_url")).toString();
    result.append(FileData{dev_serial_number, download_time, record_time, file_size, local_url});
  }
  return result;
}

QString SqliteWorker::createCondition(QVariant filterMp3,
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
