#ifndef RESTINTERFACE_H
#define RESTINTERFACE_H

#include <restconnector.h>
#include <QTextDocument>
#include "common.h"
#include "QEventLoop"

enum FileType{
    MP3,
    MP4,
    JPG,
    OTHER
};

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
public slots:

    static RestInterface* getRestInterface();
    void onFail( QNetworkReply *reply);
    void onFailPut( QNetworkReply *reply);
    void onSuccess( QNetworkReply *obj);
    void onSuccessToken( QNetworkReply *obj);
    void onSuccessToken2( QNetworkReply *obj);
    void onSuccessToken3(QNetworkReply *reply);
    void onFailToken2( QNetworkReply *obj);
    void sendPut(QVariantMap params);
    void put_hardware();
    void sendPostFile(DB_FILE_INFO* fileInfo);
    //void sendPut();
signals:
    void cookiesIsReady();
private:
    QPair<QString,FileType> getNameAndTypeFromPath(QString filePath);
    static QString fileTypeToString(FileType type);
    explicit RestInterface();
    static QList<QNetworkCookie> cookies;
    QNetworkCookie csrfToken;
    QNetworkCookie sessionId;
    RestConnector* restConnector;

    static RestInterface* restInterfacePtr;
    static QByteArray tokenTemp;
    static Token mainToken;

};

#endif // RESTINTERFACE_H
