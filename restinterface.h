#ifndef RESTINTERFACE_H
#define RESTINTERFACE_H

#include <restconnector.h>
#include "common.h"
#include "QEventLoop"


struct Token
{
    Token(QDateTime expires,
          QByteArray token,
          QByteArray sessionId) :
        expires(expires),token(token),sessionId(sessionId)
    {}
    Token(){}
    QDateTime expires;
    QByteArray token;
    QByteArray sessionId;
};

class RestInterface : public QObject
{
    Q_OBJECT
public:
    static RestInterface* getRestInterface();
public slots:
    //Бизнес логика
    void sendPutHardware(DB_DEV_INFO* devInfo); //Обновить информацию об устройстве
    void sendPostFile(DB_FILE_INFO* fileInfo); //Отослать файл
private slots:
    void onSuccessToken( QNetworkReply *obj);
    void onSuccessToken2( QNetworkReply *obj);

    //Обновление информации об устройстве
    void onSuccessPutHardware(QNetworkReply *reply);

    //Загрузка файлов
    void onSuccessPostFile( QNetworkReply *obj);

    //В случае ошибки запроса
    void onFail( QNetworkReply *reply);

signals:
    void cookiesAreReady();
private:
    enum DMT_API
    {
        API_Files,
        API_Hardwares,
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
        {API_Hardwares, "/api/registrars/"},
        {API_Login, "/accounts/login/"}
    };

    QList<QNetworkCookie> cookies;
    RestConnector* restConnector;
    bool cookiesAreBeingUpdated = false;

    RestInterface();
    static RestInterface* restInterfacePtr;

    QHttpPart generatePartForQHttpMultiPart(QString name, QByteArray value);
    QString generateAddress(DMT_API apiType);
    void sendPut(QVariantMap params);
    QPair<QString,FileType> getNameAndTypeFromPath(QString filePath);
    QString fileTypeToString(FileType type);






};

#endif // RESTINTERFACE_H
