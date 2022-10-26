#ifndef DEVICEDMT5_H
#define DEVICEDMT5_H

#include "LibBC.h"
#include "common.h"

#include <QObject>
#include <QTimer>
#include <QDateTime>

#include "sqllitecontrol.h"

class DeviceDmt5 : public QObject
{
    Q_OBJECT
public:
    explicit DeviceDmt5(QObject *parent = nullptr);

    ~DeviceDmt5();

    void InitCore();

    void StartDevice(DEVLIST_INFO handle);

    void SetDataBase(SqlLiteControl *sql_lite);

public slots:
    void DeleteDevFromMapSlot(QString sn);


private:

    void ScanDevices();


    //---------------------------------------------//

    void ConnectingDevice();

    void OpenDrive();

    void GetChargeLvl();

    void CheckDeviceInDatabase();

    void RegistrationDevice();

    bool CheckConnectDevice();

    void StartDownloadProcess();

    bool GetDriveDevice();


private:

    QMap<QString, DeviceDmt5*> *online_devices;

    SqlLiteControl *sql_lite;

    //---------------------------------------------//

    DEVLIST_INFO handle;

    DEV_INFO dev_info;

    DB_DEV_INFO db_dev_info;

    char pass[10] = "888888";

    QString drive_device;

    int battery_lvl;

    QTimer *delay_timer;

    bool download_flag = false;



signals:
    void DeleteDevFromMapSignal(QString sn);

    void startDownloadSignal(QString serial_number, QString drive_device);
};

#endif // DEVICEDMT5_H
