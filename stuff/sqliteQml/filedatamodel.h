#ifndef FILEDATAMODEL_H
#define FILEDATAMODEL_H

#include <QAbstractTableModel>
#include <QDateTime>
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include "sqliteworker.h"
#include <qqml.h>
#include <QtQml>
#include <QTimer>
//Класс мождели файлов, хранит данные о файлах их бд в контейнере fileDataList
class FileDataModel : public QAbstractTableModel
{
  Q_OBJECT

  Q_PROPERTY(QVariant filterMp3 READ getFilterMp3 WRITE setFilterMp3 NOTIFY modelNeedChange)
  Q_PROPERTY(QVariant filterMp4 READ getFilterMp4 WRITE setFilterMp4 NOTIFY modelNeedChange)
  Q_PROPERTY(QVariant filterJpg READ getFilterJpg WRITE setFilterJpg NOTIFY modelNeedChange)
  Q_PROPERTY(QVariant filterDateRecordStart READ getFilterDateRecordStart WRITE setFilterDateRecordStart NOTIFY modelNeedChange)
  Q_PROPERTY(QVariant filterDateRecordEnd READ getFilterDateRecordEnd WRITE setFilterDateRecordEnd NOTIFY modelNeedChange)
  Q_PROPERTY(QVariant filterDateDownloadStart READ getFilterDateDownloadStart WRITE setFilterDateDownloadStart NOTIFY modelNeedChange)
  Q_PROPERTY(QVariant filterDateDownloadEnd READ getFilterDateDownloadEnd WRITE setFilterDateDownloadEnd NOTIFY modelNeedChange)
  Q_PROPERTY(QVariant filterSerialNumber READ getSerialNumber WRITE setSerialNumber NOTIFY modelNeedChange)

  //Q_PROPERTY(QString filter READ filter WRITE setFilter NOTIFY filterChanged)
  Q_PROPERTY(int pageSize READ getPageSize WRITE setPageSize NOTIFY pageSizeChanged)
  Q_PROPERTY(int pageNum READ getPageNum WRITE setPageNum NOTIFY pageNumChanged)
  QML_ELEMENT
public:
  FileDataModel();
  int rowCount(const QModelIndex &parent) const;
  int columnCount(const QModelIndex &parent) const;
  QVariant data(const QModelIndex &index, int role) const;
  bool setData(const QModelIndex &index, const QVariant &value, int role);
  bool removeRows(int row, int count, const QModelIndex &parent);
  Q_INVOKABLE QVariant headerData(int section, Qt::Orientation orientation, int role) const;
  //Функция добавления нового элемента в модель (В конец внутреннего контейнера). Используется при заполнении из бд.
  void appendItem(FileData file);
  //Функция конвертации данных из модели в json
  QString toJSON();
  // get set функции для измненения размера выгрузок из бд в соответствии с размерами страниц на QML
  void setPageSize(int size);
  void setPageNum(int num);
  int getPageSize();
  int getPageNum();
  // get set функции для заполнения фильтров
  void setFilterMp3(QVariant filterMp3){
    this->filterMp3 = filterMp3;
    emit modelNeedChange();
  }
  QVariant getFilterMp3(){return filterMp3;}

  void setFilterMp4(QVariant filterMp4){
    this->filterMp4 = filterMp4;
    emit modelNeedChange();
  }
  QVariant getFilterMp4(){return filterMp4;}

  void setFilterJpg(QVariant filterJpg){
    this->filterJpg = filterJpg;
    emit modelNeedChange();
  }
  QVariant getFilterJpg(){return filterJpg;}

  void setFilterDateRecordStart(QVariant filterDateRecordStart){
    this->filterDateRecordStart = filterDateRecordStart;
    emit modelNeedChange();
  }
  QVariant getFilterDateRecordStart(){return filterDateRecordStart;}
  void setFilterDateRecordEnd(QVariant filterDateRecordEnd){
    this->filterDateRecordEnd = filterDateRecordEnd;
    emit modelNeedChange();
  }
  QVariant getFilterDateRecordEnd(){return filterDateRecordEnd;}

  void setFilterDateDownloadStart(QVariant filterDateDownloadStart){
    this->filterDateDownloadStart = filterDateDownloadStart;
    emit modelNeedChange();
  }
  QVariant getFilterDateDownloadStart(){return filterDateDownloadStart;}
  void setFilterDateDownloadEnd(QVariant filterDateDownloadEnd){
    this->filterDateDownloadEnd = filterDateDownloadEnd;
    emit modelNeedChange();
  }
  QVariant getFilterDateDownloadEnd(){return filterDateDownloadEnd;}

  void setSerialNumber(QVariant filterSerialNumber){
    this->filterSerialNumber = filterSerialNumber;
    emit modelNeedChange();
  }
  QVariant getSerialNumber(){return filterSerialNumber;}

signals:
  void pageSizeChanged();
  void pageNumChanged();

  void modelNeedChange();
public slots:
  Q_INVOKABLE void updateSelf();
private:
  //Контейнер для хранения объектов из бд
  QList<FileData> fileDataList;
  //Строка фильтров
  QString filter_s="";
  //Число записей на 1 странице для loadFiles
  int pageSize = 5;
  //Номер текущей страницы страницы для loadFiles
  int pageNum = 0;
  //Параметры по которым прводится фильтрация
  QVariant filterMp3 = "";
  QVariant filterMp4 = "";
  QVariant filterJpg = "";
  QVariant filterDateRecordStart = "";
  QVariant filterDateRecordEnd = "";
  QVariant filterDateDownloadStart = "";
  QVariant filterDateDownloadEnd = "";
  QVariant filterSerialNumber = "";
};

#endif // FILEDATAMODEL_H
