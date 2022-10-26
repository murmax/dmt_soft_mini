#include "devicedmt5.h"

#include <QDebug>

#include <windows.h>
#include <fileapi.h>

DeviceDmt5::DeviceDmt5(QObject *parent)
    : QObject{parent}
{
    delay_timer = new QTimer();
}

DeviceDmt5::~DeviceDmt5()
{
    delay_timer->stop();
    delay_timer->deleteLater();
    emit DeleteDevFromMapSignal(QString(handle.SN));
    this->deleteLater();
}

void DeviceDmt5::InitCore()
{
    QMap<QString, DeviceDmt5*> online_devices;
    this->online_devices = &online_devices;

    emit ScanDevices();
}

void DeviceDmt5::ScanDevices()
{
    DEVLIST_INFO device_handles[30];

    BC_InitDevListPthreadEx(NULL);

    int amount = BC_GetDevAllListInfoFromList(device_handles, 30);

    for(int i=0; i < amount; i++){
        if(!online_devices->contains(device_handles[i].SN)) {
            DeviceDmt5 *device = new DeviceDmt5();
            device->SetDataBase(sql_lite);
            connect(device, &DeviceDmt5::DeleteDevFromMapSignal, this, &DeviceDmt5::DeleteDevFromMapSlot);
            online_devices->insert(QString(device_handles[i].SN), device);
            connect(device, &DeviceDmt5::startDownloadSignal, this, &DeviceDmt5::startDownloadSignal);
            device->StartDevice(device_handles[i]);
        }
    }
    qDebug() << online_devices->size() << " SIZE";
    delay_timer->singleShot(1000, this, &DeviceDmt5::ScanDevices);
}


void DeviceDmt5::StartDevice(DEVLIST_INFO handle)
{
    this->handle = handle;
    ConnectingDevice();
    qDebug() << "INFO:START_NEW_DEV";
    if(sql_lite->GetDevInfo(QString(handle.SN), &db_dev_info)) {
        qDebug() << "INFO:DEV_IS_READY";
    }
    else {
        sql_lite->AddNewDev(QString(handle.SN));
        qDebug() << "INFO:DEV_IS_REGISTRATION";
    }

    if(!GetDriveDevice()){
        OpenDrive();
        qDebug() << "INFO:DEV_DRIVE_OPEN";
    }

    CheckConnectDevice();


}

void DeviceDmt5::SetDataBase(SqlLiteControl *sql_lite)
{
    this->sql_lite = sql_lite;
}

void DeviceDmt5::DeleteDevFromMapSlot(QString sn)
{
    online_devices->remove(sn);
    qDebug() << "INFO:DEV_DISCONNECT";
}

void DeviceDmt5::ConnectingDevice()
{
    BC_Login(handle.dev, pass);

    BC_GetDevInfo(handle.dev, pass, &dev_info);
}

void DeviceDmt5::OpenDrive()
{
    BC_EnterDiskMode(handle.dev, pass);

    delay_timer->singleShot(3000, this, &DeviceDmt5::GetDriveDevice);
}

void DeviceDmt5::GetChargeLvl()
{
    int battery_lvl;

    BC_GetBatVal(handle.dev, pass, &battery_lvl);

    if (battery_lvl > 9 && battery_lvl < 101 && battery_lvl%10 == 0)
    {
        this->battery_lvl = battery_lvl;
    }
}

bool DeviceDmt5::CheckConnectDevice()
{
    if (GetDriveDevice()) {
        GetChargeLvl();
        sql_lite->SetDevBatteryLvl(QString(handle.SN), battery_lvl);
        sql_lite->SetOnlineTime(QString(handle.SN), QDateTime::currentDateTime().toString("yyyy-MM-dd mm:hh:ss"));
        delay_timer->singleShot(2000, this, &DeviceDmt5::CheckConnectDevice);
    }
    else {
        this->~DeviceDmt5();
    }
}

void DeviceDmt5::CheckDeviceInDatabase()
{

}

void DeviceDmt5::RegistrationDevice()
{

}

void DeviceDmt5::StartDownloadProcess()
{

}

bool DeviceDmt5::GetDriveDevice()
{
    char Buf[300]{0};
    QVector<char> drives;

    char NameBuffer[MAX_PATH];
    char SysNameBuffer[MAX_PATH];
    DWORD VSNumber;
    DWORD MCLength;
    DWORD FileSF;

    GetLogicalDriveStrings(300, LPWSTR(Buf));
    for (int i = 0; i < 300; i += 8) {
        if (isalpha(Buf[i])) {
            drives.push_back(Buf[i]);
        }
    }

    QString id_device = QString::fromLocal8Bit(dev_info.cSerial);

    for(int i = 0; i < drives.size(); i++)
    {
        std::string drive = "";
        drive = drive + drives[i] + ":\\";

        GetVolumeInformationA(drive.c_str(), NameBuffer, sizeof(NameBuffer), &VSNumber,&MCLength,&FileSF,SysNameBuffer,sizeof(SysNameBuffer));

        QString found_id_device = QString::fromLocal8Bit(NameBuffer);

        if(found_id_device == id_device){
            drive_device = QString::fromLocal8Bit(drive.c_str());

            if(!download_flag) {
                download_flag = true;
                emit startDownloadSignal(QString(handle.SN), drive_device);
                qDebug() << "INFO:DEV_START_DOWNLOAD";
            }

            return true;
        }
    }
    return false;
}
