#ifndef RESTINTERFACE_H
#define RESTINTERFACE_H

#include <restconnector.h>
#include "sqllitecontrol.h"
#include "common.h"
#include "QEventLoop"

class RestInterface : public QObject
{
    Q_OBJECT
public:
    static RestInterface* getRestInterface();
public slots:
    //Бизнес логика
    void sendPutDev(DevInfo* devInfo); //Обновить информацию об устройстве
    void sendPostFile(DB_FILE_INFO* fileInfo); //Отослать файл
private slots:
    void onSuccessToken( QNetworkReply *reply);
    void onSuccessToken2( QNetworkReply *reply);

    //Обновление информации об устройстве
    void onSuccessPutDev(QNetworkReply *reply);

    //Загрузка файлов
    void onSuccessPostFile( QNetworkReply *reply);
    void onFailPostFile( QNetworkReply *reply);

    //В случае ошибки запроса
    void onFail( QNetworkReply *reply);

signals:
    void cookiesAreReady();

    void fileSentSuccess(QString url);
    void fileSentFail(QString url);
private:
    enum DMT_API
    {
        API_Files,
        API_Devs,
        API_Login
    };
    enum FileType{
        MP3,
        MP4,
        JPG,
        OTHER
    };
    const QString host = "http://134.0.116.177";
    const QMap<DMT_API,QString> apiAddress =
    {
        {API_Files, "/api/files/"},
        {API_Devs, "/api/registrars/"},
        {API_Login, "/accounts/login/"}
    };

    QList<QNetworkCookie> cookies;
    RestConnector* restConnector;
    QMap<QNetworkReply*,QString> filesPosted;
    bool cookiesAreBeingUpdated = false;
    QByteArray login = qCompress("qwerty123@gmail.com",9);
    QByteArray password = qCompress("1234",9);


    RestInterface();
    static RestInterface* restInterfacePtr;

    QHttpPart generatePartForQHttpMultiPart(QString name, QByteArray value);
    QString generateAddress(DMT_API apiType);
    void sendPutDevParams(QVariantMap params,QString dev_serial_number);
    QPair<QString,FileType> getNameAndTypeFromPath(QString filePath);
    QString fileTypeToString(FileType type);

};

#endif // RESTINTERFACE_H
