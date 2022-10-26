#include "downloadcontrol.h"

DownloadControl::DownloadControl(QObject *parent)
    : QObject{parent}
{
    qDebug() << "INFO:DOWNMAN_INIT";
}

void DownloadControl::SetDataBase(SqlLiteControl *sql_lite)
{
    this->sql_lite = sql_lite;
}

void DownloadControl::startDownloadSlot(QString serial_number, QString drive_device)
{
    qDebug() << "INFO:DOWNMAN_START_DOWNLOAD";
    QtConcurrent::run(this, &DownloadControl::startDownloadInNewThread, serial_number, drive_device);
}

void DownloadControl::startDownloadInNewThread(QString serial_number, QString drive_device)
{
    QDir dir = QDir(drive_device);
    QFileInfoList files_list;
    getFilesList(dir, files_list);
    downloadFiles(serial_number, files_list);
}

int DownloadControl::getFilesList(QDir drive_device, QFileInfoList &files_list) {
    int byte_size = 0;

    foreach(QFileInfo file, drive_device.entryInfoList(QDir::Files|QDir::Dirs|QDir::NoDotAndDotDot, QDir::Name|QDir::DirsFirst)) {

        if(file.isDir()) {
            drive_device.cd(file.fileName());
            getFilesList(drive_device, files_list);
            drive_device.cdUp();
        } else {
            files_list.append(file);
            byte_size += file.size();
        }
    }
    return byte_size;
}

bool DownloadControl::downloadFiles(QString serial_number, QFileInfoList &files_list)
{
    QSettings settings;

    QString local_path = settings.value("settings/local_path", "D://DmtData").toString();
    //QString server_path = settings.value("settings/server_info", "D://DmtData").toString();

    int session_file_number = 0;
    foreach(QFileInfo file, files_list)
    {
        QString down_path = local_path;
        down_path += "/";
        down_path += QDateTime::currentDateTime().toString("yyyy");
        down_path += "/";
        down_path += QDateTime::currentDateTime().toString("MM");
        down_path += "/";
        down_path += QDateTime::currentDateTime().toString("dd");
        down_path += "/";
        down_path += QDateTime::currentDateTime().toString("hh");

        QDir().mkpath(down_path);

        down_path += "/";
        down_path += serial_number;
        down_path += "_";
        down_path += QDateTime::currentDateTime().toString("mmss");
        down_path += "_";
        down_path += QString::number(session_file_number);
        down_path += ".";
        down_path += file.suffix();

        qDebug() << down_path;

        qDebug() << QFile::copy(file.absoluteFilePath(), down_path);

        session_file_number++;

        sql_lite->AddNewFile(serial_number, file.lastModified().toString(), down_path, QString::number(file.size()));

        QFile::remove(file.absoluteFilePath());
    }
}

