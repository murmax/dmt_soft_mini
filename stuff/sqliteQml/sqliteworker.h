#ifndef SQLITEWORKER_H
#define SQLITEWORKER_H

#include <QtSql>
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>
#include <QTimer>
#include <QtQml/QQmlEngine>
#include <QtQuick/QQuickView>



//структура для хранения данных из бд
struct FileData {
  QString dev_serial_number;
  QDateTime download_time;
  QDateTime record_time;
  QString file_size;
  QString local_url;
  FileData(){}
  FileData(   QString dev_serial_number,
              QDateTime download_time,
              QDateTime record_time,
              QString file_size,
              QString local_url)
  {
    this->dev_serial_number = dev_serial_number;
    this->download_time = download_time;
    this->record_time = record_time;
    this->file_size = file_size;
    this->local_url = local_url;
  }
};

struct HardwareData {
  QString dev_serial_number;
  QDateTime registration_time;
  QDateTime last_online_time;
  int battery_lvl;
  int download_lvl;
  HardwareData();
  HardwareData(   QString dev_serial_number,
                  QDateTime registretion_time,
                  QDateTime last_online_time,
                  int battery_lvl,
                  int download_lvl)
  {
    this->dev_serial_number = dev_serial_number;
    this->registration_time = registretion_time;
    this->last_online_time = last_online_time;
    this->battery_lvl = battery_lvl;
    this->download_lvl = download_lvl;

  }
};

//Класс взаимодействия с бд
class SqliteWorker : public QObject
{
  Q_OBJECT
  QML_ELEMENT
public:

  QQuickView ui;
  SqliteWorker();
  //Singleton
  static SqliteWorker *getSqliteWorker();
  //Функция подключения к бд
  bool connectToDataBase();
  //Модель для хранения информации об оборудовании
  //HardwareDataModel hardwareModel; //Временно в public
  //Модель для хранения информации о файлах
  // FileDataModel fileModel; //Временно в public
  //set и get функции для доступа к парметрам PageNum и PageSize
  QList<HardwareData> loadHardware();
  QList<FileData> loadFiles(int pageSize,
                            int pageNum,
                            QVariant filterMp3,
                            QVariant filterMp4,
                            QVariant filterJpg,
                            QVariant filterDateRecordStart,
                            QVariant filterDateRecordEnd,
                            QVariant filterDateDownloadStart,
                            QVariant filterDateDownloadEnd,
                            QVariant filterSerialNumber);
private:
  static SqliteWorker* sqliteworker_ptr;
  //Объект для связи с бд
  QSqlDatabase dbase;
  //Таймер синхронизации с бд
  QTimer* databaseResyncTimer;
  QString createCondition(QVariant filterMp3,
                          QVariant filterMp4,
                          QVariant filterJpg,
                          QVariant filterDateRecordStart,
                          QVariant filterDateRecordEnd,
                          QVariant filterDateDownloadStart,
                          QVariant filterDateDownloadEnd,
                          QVariant filterSerialNumber);
  //Функция получения информации об оборудовании
  //Функция получения информации о файлах
  //condition - Условие после WHERE в виде sql строки. Примеры валидных условий:

private slots:
  //Функция синхронизации с БД
  void syncWithDB();
};

#endif // SQLITEWORKER_H
