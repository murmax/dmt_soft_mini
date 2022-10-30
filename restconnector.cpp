#include "restconnector.h"
#include "restinterface.h"

RestConnector* RestConnector::restConnectorPtr = nullptr;

const QString RestConnector::httpTemplate = "http://%1/%2";
const QString RestConnector::httpsTemplate = "https://%1/%2";
const QString RestConnector::KEY_QNETWORK_REPLY_ERROR = "QNetworkReplyError";
const QString RestConnector::KEY_CONTENT_NOT_FOUND = "ContentNotFoundError";


RestConnector::RestConnector(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
}

RestConnector *RestConnector::getRestConnector()
{
    if (!restConnectorPtr)
    {
        restConnectorPtr = new RestConnector();
    }
    return restConnectorPtr;
}

void RestConnector::initRequester( QSslConfiguration *value)
{
    sslConfig = value;
}

QNetworkReply *RestConnector::sendRequest(const QString &apiStr,
                                const handleFuncReply &funcSuccess,
                                const handleFuncReply &funcError,
								QList<QNetworkCookie> cookies,
                                RestConnector::Type type,
                                const QVariantMap &data,
                                QHttpMultiPart *multiPart)
{
    QNetworkRequest request = createRequest(apiStr);
    if (cookies.length()>0)
        request.setHeader(QNetworkRequest::CookieHeader, QVariant::fromValue(cookies));


    QNetworkReply *reply = nullptr;
    switch (type) {
    case Type::POST_MULTI: {
        if (multiPart!=nullptr){
            reply = manager->post(request, multiPart);
            multiPart->setParent(reply); // delete the multiPart with the reply
        }
        break;
    }
    case Type::POST: {
        QString postDataByteArray;

        for (auto iter = data.begin();iter!=data.end();iter++)
        {
            postDataByteArray+=iter.key()+"="+iter.value().toByteArray()+"&";
        }
        if (postDataByteArray.endsWith('&'))
            postDataByteArray=postDataByteArray.left(postDataByteArray.length()-1);
        reply = manager->post(request, postDataByteArray.toUtf8());
        break;
    } case Type::GET: {
        reply = manager->get(request);
        break;
    } case Type::PUT: {
        if (multiPart!=nullptr){
            reply = manager->put(request, multiPart);
            multiPart->setParent(reply); // delete the multiPart with the reply
        }
        break;
    } case Type::DELET: {
        if (data.isEmpty())
            reply = manager->deleteResource(request);
        else
            reply = sendCustomRequest(manager, request, "DELETE", data);
        break;
    } case Type::PATCH: {
        reply = sendCustomRequest(manager, request, "PATCH", data);
        break;
    } default:
        reply = nullptr;
        Q_ASSERT(false);
    }

    connect(reply, &QNetworkReply::finished, this,
            [this, funcSuccess, funcError, reply]() {
        if (reply){
            if (onFinishRequest(reply)) {
                if (funcSuccess != nullptr) {
                    funcSuccess(RestInterface::getRestInterface(),reply);
                }
            } else {
                if (funcError != nullptr) {
                    handleQtNetworkErrors(reply);
                    funcError(RestInterface::getRestInterface(),reply);
                }
            }
            reply->close();
            reply->deleteLater();
        }
    } );
    return reply;

}


QString RestConnector::getToken() const
{
    return token;
}

void RestConnector::setToken(const QString &value)
{
    token = value;
}

QByteArray RestConnector::variantMapToJson(QVariantMap data)
{
    QJsonDocument postDataDoc = QJsonDocument::fromVariant(data);
    QByteArray postDataByteArray = postDataDoc.toJson();

    return postDataByteArray;
}

QNetworkRequest RestConnector::createRequest(const QString &apiStr)
{
    QNetworkRequest request;
    QString url = apiStr;
    request.setUrl(QUrl(url));

    if(!token.isEmpty())
        request.setRawHeader("Authorization",QString("OAuth %1").arg(token).toUtf8());
    if (sslConfig != nullptr)
        request.setSslConfiguration(*sslConfig);

    return request;
}

QNetworkReply* RestConnector::sendCustomRequest(QNetworkAccessManager* manager,
                                                QNetworkRequest &request,
                                                const QString &type,
                                                const QVariantMap &data)
{
    request.setRawHeader("HTTP", type.toUtf8());
    QByteArray postDataByteArray = variantMapToJson(data);
    QBuffer *buff = new QBuffer;
    buff->setData(postDataByteArray);
    buff->open(QIODevice::ReadOnly);
    QNetworkReply* reply =  manager->sendCustomRequest(request, type.toUtf8(), buff);
    buff->setParent(reply);
    return reply;
}

bool RestConnector::onFinishRequest(QNetworkReply *reply)
{
    auto replyError = reply->error();
    if (replyError == QNetworkReply::NoError ) {
        int code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if ((code >=200) && (code < 400)) {
            return true;
        }
    }
    return false;
}

void RestConnector::handleQtNetworkErrors(QNetworkReply *reply)
{
    qDebug() << reply->error();
}
