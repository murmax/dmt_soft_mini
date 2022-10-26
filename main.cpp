#include <QCoreApplication>
#include <QObject>

#include "devicedmt5.h"
#include "sqllitecontrol.h"
#include "downloadcontrol.h"

#include <QVariantMap>

int main(int argc, char *argv[])
{    
    QCoreApplication a(argc, argv);

    QCoreApplication::setOrganizationName("DMT");
    QCoreApplication::setApplicationName("DmtSoftMini");

    SqlLiteControl *sql_lite = new SqlLiteControl();

    DownloadControl *download_manager = new DownloadControl();
    download_manager->SetDataBase(sql_lite);

    DeviceDmt5 *dmt5_controller = new DeviceDmt5();
    QObject::connect(dmt5_controller, &DeviceDmt5::startDownloadSignal, download_manager, &DownloadControl::startDownloadSlot);
    dmt5_controller->SetDataBase(sql_lite);

    return a.exec();


}
