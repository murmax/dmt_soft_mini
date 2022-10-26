#ifndef MAINCLASS_H
#define MAINCLASS_H

#include <QObject>
#include "devicedmt5.h"
#include "sqllitecontrol.h"
#include "downloadcontrol.h"

class MainClass : public QObject
{
    Q_OBJECT
public:
    explicit MainClass(QObject *parent = nullptr);

    void InitMain();
signals:

private:
    SqlLiteControl *sql_lite;

    DownloadControl *download_manager;

    DeviceDmt5 *dmt5_controller;

};

#endif // MAINCLASS_H
