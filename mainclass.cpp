#include "mainclass.h"

MainClass::MainClass(QObject *parent)
    : QObject{parent}
{

}

void MainClass::InitMain()
{
    sql_lite = new SqlLiteControl();

    download_manager = new DownloadControl();
    download_manager->SetDataBase(sql_lite);

    dmt5_controller = new DeviceDmt5();

    qDebug() << QObject::connect(dmt5_controller, &DeviceDmt5::startDownloadSignal, download_manager, &DownloadControl::startDownloadSlot);
    dmt5_controller->SetDataBase(sql_lite);
    dmt5_controller->InitCore();
}
