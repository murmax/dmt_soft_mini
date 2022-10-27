#include "restconnector.h"

#include "restconnector.h"

RestConnector* RestConnector::restConnectorPtr = nullptr;

const QString RestConnector::httpTemplate = "http://%1/%2";
const QString RestConnector::httpsTemplate = "https://%1/%2";
const QString RestConnector::KEY_QNETWORK_REPLY_ERROR = "QNetworkReplyError";
const QString RestConnector::KEY_CONTENT_NOT_FOUND = "ContentNotFoundError";

const QString yandexDiskAddr = "https://cloud-api.yandex.net/v1/disk";

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

void RestConnector::initRequester(const QString &host,  QSslConfiguration *value)
{
    this->host = host;
    sslConfig = value;
    if (sslConfig != nullptr)
        pathTemplate = httpsTemplate;
    else
        pathTemplate = httpTemplate;
}

void RestConnector::sendRequest(const QString &apiStr,
                                const handleFuncReply &funcSuccess,
                                const handleFuncReply &funcError,
								QList<QNetworkCookie> cookies,
                                RestConnector::Type type,
                                const QVariantMap &data,
                                QHttpMultiPart *multiPart)
{
    QNetworkRequest request = createRequest(apiStr);
    request.setHeader(QNetworkRequest::CookieHeader, qVariantFromValue(cookies));
    qDebug() << "Cookie:" << request.header(QNetworkRequest::CookieHeader).value<QList<QNetworkCookie>>();
    //qDebug()<<"Request. URL:"<<request.url()<<"headers:"<<request.rawHeaderList();

    QNetworkReply *reply;
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
        //QByteArray postDataByteArray = variantMapToJson(data);
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
        /*
        if (data.contains("raw"))
        {
            reply = manager->put(request,data.value("raw").toByteArray());
        } else
        {
            reply = manager->put(request,"Some random data");
        }
        */
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
        //QJsonObject obj = parseReply(reply);
        //QByteArray replyText = reply->readAll();
        if (onFinishRequest(reply)) {
            if (funcSuccess != nullptr)
                funcSuccess(reply);
        } else {
            if (funcError != nullptr) {
                handleQtNetworkErrors(reply);
                funcError(reply);
            }
        }
        reply->close();
        reply->deleteLater();
    } );

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

QJsonObject RestConnector::parseReply(QNetworkReply *reply)
{
    QJsonObject jsonObj;
    QJsonDocument jsonDoc;
    QJsonParseError parseError;
    QByteArray replyText = reply!=nullptr ? reply->readAll() : QByteArray();
    jsonDoc = QJsonDocument::fromJson(replyText, &parseError);
    if(parseError.error != QJsonParseError::NoError){
        qDebug() << replyText;
        qWarning() << "Json parse error: " << parseError.errorString();
    }else{
        if(jsonDoc.isObject())
            jsonObj  = jsonDoc.object();
        else if (jsonDoc.isArray())
            jsonObj["non_field_errors"] = jsonDoc.array();
    }
    return jsonObj;
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
