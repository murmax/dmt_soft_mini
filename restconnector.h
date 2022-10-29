#ifndef RESTCONNECTOR_H
#define RESTCONNECTOR_H


#include <QObject>
#include <QBuffer>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QHttpMultiPart>
#include <QFile>
#include <functional>
#include <QNetworkCookie>
class RestInterface;

class RestConnector : public QObject
{
    Q_OBJECT
public:
    static RestConnector* getRestConnector();
    //typedef std::function<void(const QByteArray &)> handleFunc;
    typedef std::function<void(RestInterface*,QNetworkReply*)> handleFuncReply;
    typedef std::function<void()> finishFunc;

    static const QString KEY_QNETWORK_REPLY_ERROR;
    static const QString KEY_CONTENT_NOT_FOUND;

    enum class Type {
        POST,
        GET,
        PATCH,
        DELET,
        PUT,
        POST_MULTI
    };


    void initRequester(QSslConfiguration *value);

    void sendRequest(const QString &apiStr,
                     const handleFuncReply &funcSuccess,
                     const handleFuncReply &funcError,
                     QList<QNetworkCookie> cookies,
                     Type type = Type::GET,
                     const QVariantMap &data = QVariantMap(),
                     QHttpMultiPart *multiPart=nullptr);

    QString getToken() const;
    void setToken(const QString &value);

private:
    explicit RestConnector(QObject *parent = nullptr);
    static RestConnector* restConnectorPtr;
    static const QString httpTemplate;
    static const QString httpsTemplate;

    QString token;
    QSslConfiguration *sslConfig;


    QByteArray variantMapToJson(QVariantMap data);

    QNetworkRequest createRequest(const QString &apiStr);

    QNetworkReply *sendCustomRequest(QNetworkAccessManager *manager,
                                     QNetworkRequest &request,
                                     const QString &type,
                                     const QVariantMap &data);


    bool onFinishRequest(QNetworkReply *reply);

    void handleQtNetworkErrors(QNetworkReply *reply);
    QNetworkAccessManager *manager;

signals:
    void networkError();
};


#endif // RESTCONNECTOR_H
