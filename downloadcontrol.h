#ifndef DOWNLOADCONTROL_H
#define DOWNLOADCONTROL_H

#include <QObject>
#include <QDebug>
#include <QSettings>

#include <QDir>
#include <QDateTime>

#include <QtConcurrent/QtConcurrent>
#include <QtConcurrent/QtConcurrentRun>

#include "sqllitecontrol.h"

#include "common.h"

class DownloadControl : public QObject
{
    Q_OBJECT
public:
    explicit DownloadControl(QObject *parent = nullptr);

    void SetDataBase(SqlLiteControl *sql_lite);

public slots:
    void startDownloadSlot(QString serial_number, QString drive_device);

private:
    void startDownloadInNewThread(QString serial_number,QString drive_device);

    int getFilesList(QDir drive_device, QFileInfoList &files_list);

    bool downloadFiles(QString serial_number, QFileInfoList &files_list);





private:
    QString path;

    SqlLiteControl *sql_lite;
};

#endif // DOWNLOADCONTROL_H
