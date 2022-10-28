#include "restinterface.h"
#include <QNetworkReply>
#include <QNetworkCookie>
#include <QTimer>

QByteArray RestInterface::tokenTemp;
Token RestInterface::mainToken;
QList<QNetworkCookie> RestInterface::cookies;
RestInterface* RestInterface::restInterfacePtr = nullptr;

RestInterface::RestInterface() : QObject(nullptr)
{
    restInterfacePtr = this;
    restConnector = RestConnector::getRestConnector();
    restConnector->initRequester("cloud-api.yandex.net",new QSslConfiguration());

//    if (cookies.isEmpty()){
//        restConnector->sendRequest("http://134.0.116.177/accounts/login/",&RestInterface::onSuccessToken,&RestInterface::onFail,cookies,RestConnector::Type::GET,QVariantMap());
//    }

    /*
    DB_FILE_INFO* fileInfo = new DB_FILE_INFO();
    fileInfo->dev_serial_number="123";
    fileInfo->local_url="C:/Users/Dmitry/Desktop/123.jpg";
    fileInfo->size="3";
    fileInfo->download_time="2018-10-11";
    fileInfo->record_time="2018-10-12";
    sendPostFile(fileInfo);
    */
    qDebug() << "params123";
    QVariantMap params;
    params.insert("power",10);
    params.insert("download",11);
    sendPut(params);
}

void RestInterface::onSuccess( QNetworkReply *reply)
{
    qDebug()<<__func__<<reply->readAll();
}

void RestInterface::onSuccessToken( QNetworkReply *reply)
{
    qDebug() << "onSuccessToken";
    QString res = reply->readAll();
    int len1 = res.indexOf("csrfmiddlewaretoken")+QString("csrfmiddlewaretoken").length();
    int len2 = res.indexOf("value",len1)+QString("value").length();
    int len3 = res.indexOf("\"",len2)+QString("\"").length();
    int len4 = res.indexOf("\"",len3);
    tokenTemp = res.mid(len3,len4-len3).toUtf8();
    qDebug()<<"csrfmiddlewaretoken:"<<tokenTemp;

    if (!tokenTemp.isEmpty()){
        QVariantMap data;
        data.insert("csrfmiddlewaretoken",tokenTemp);
        data.insert("username","qwerty123@gmail.com ");
        data.insert("password","1234");
        RestConnector::getRestConnector()->sendRequest
                ("http://134.0.116.177/accounts/login/",&RestInterface::onSuccessToken2,&RestInterface::onFailToken2,cookies,RestConnector::Type::POST,data);
    }
}

void RestInterface::onSuccessToken2( QNetworkReply *reply)
{
    QList<QNetworkCookie> cookies1 = reply->header(QNetworkRequest::SetCookieHeader).value<QList<QNetworkCookie>>();
    cookies = cookies1;
    qDebug()<<Qt::endl<<"result success:"<<reply->readAll();
    for (auto e : cookies1)
    {
        qDebug()<<"Cookies:"<<e;
    }
    emit cookiesIsReady();
    /*
    QVariantMap params =
    {
        {"power","62"},
        {"download","62"}
    };
    sendPut(params);
    */


}

void RestInterface::onSuccessToken3(QNetworkReply *reply)
{
    qDebug()<<"result success:"<<reply->readAll();
}

void RestInterface::onFailToken2( QNetworkReply *reply)
{
    qDebug()<<"result fail:"<<reply->readAll();
}


void RestInterface::sendPut(QVariantMap params)
{
    if (cookies.isEmpty()){
        qDebug() << "cookies is empty!";
        QEventLoop loop;
        connect(this, &RestInterface::cookiesIsReady, &loop, &QEventLoop::quit);
        restConnector->sendRequest("http://134.0.116.177/accounts/login/",&RestInterface::onSuccessToken,&RestInterface::onFail,cookies,RestConnector::Type::GET,QVariantMap());
        qDebug() << "here";
        loop.exec();
    }
    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    auto generatePart = [](QString name, QByteArray value) -> QHttpPart
    {
        QHttpPart part;
        part.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\""+name+"\""));
        part.setBody(value);
        return part;
    };
    QList<QHttpPart> form;
    for (auto iter = params.begin();iter!=params.end();iter++)
    {
        form.append(generatePart(iter.key(),iter.value().toByteArray()));
    }

    for (const auto &part : form)
    {
        multiPart->append(part);
    }
    RestConnector::getRestConnector()->sendRequest
            ("http://134.0.116.177/api/registrars/00000000/",&RestInterface::onSuccessToken3,&RestInterface::onFailPut,cookies,RestConnector::Type::PUT,QVariantMap(), multiPart);
}

RestInterface *RestInterface::getRestInterface()
{
    if(restInterfacePtr == nullptr){
        restInterfacePtr = new RestInterface();
    }
    return restInterfacePtr;
}

void RestInterface::onFail( QNetworkReply *reply)
{
    qDebug() << "onFail";
    qDebug()<<__func__<<reply->readAll();
    cookies.clear();
}

void RestInterface::onFailPut(QNetworkReply *reply)
{
    qDebug()<<__func__<<reply->readAll();
    cookies.clear();
}

void RestInterface::sendPostFile(DB_FILE_INFO* fileInfo)
{
    if(fileInfo != nullptr){
        QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
        auto generatePart = [](QString name, QByteArray value) -> QHttpPart
        {
            QHttpPart part;
            part.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\""+name+"\""));
            part.setBody(value);
            return part;
        };
        QList<QHttpPart> form;
        form.append(generatePart("registrar","00000000"));

        QHttpPart imagePart;
        imagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/jpeg"));
        imagePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"source\"; filename=\"123.jpg\""));
        QFile *file = new QFile("C:\\Users\\Dmitry\\Desktop\\123.jpg");
        file->open(QIODevice::ReadOnly);
        imagePart.setBodyDevice(file);
        file->setParent(multiPart);
        form.append(imagePart);

        QPair<QString, FileType> pair = getNameAndTypeFromPath(fileInfo->local_url);

        form.append(generatePart("name",pair.first.toLatin1()));
        form.append(generatePart("size",fileInfo->size.toLatin1()));
        form.append(generatePart("type",fileTypeToString(pair.second).toLatin1()));
        form.append(generatePart("record_date",fileInfo->record_time.toLatin1()));
        form.append(generatePart("unload_date",fileInfo->download_time.toLatin1()));
        form.append(generatePart("owner","Test_max"));

        for (const auto &part : form)
        {
            multiPart->append(part);
        }
        RestConnector::getRestConnector()->sendRequest("http://134.0.116.177/api/files/",&RestInterface::onSuccess,&RestInterface::onFail,cookies,RestConnector::Type::POST_MULTI,QVariantMap(),multiPart);
    }
}

QPair<QString, FileType> RestInterface::getNameAndTypeFromPath(QString filePath)
{
    QPair<QString, FileType> result = QPair<QString, FileType>("",FileType::OTHER);
    QStringList buff1 = filePath.split("/");
    if(buff1.length() == 0){return result;}
    QString fileName = buff1.last();

    QStringList buff2 = filePath.split(".");
    if(buff2.length() == 0){return result;}
    QString fileType = buff2.last();

    result.first = fileName;
    if(fileType == "mp3"){
        result.second = FileType::MP3;
    }else if(fileType == "mp4"){
        result.second = FileType::MP4;
    }
    else if((fileType == "jpg") || (fileType == "jpeg")){
        result.second = FileType::JPG;
    }else{
        result.second = FileType::OTHER;
    }

    return result;
}

QString RestInterface::fileTypeToString(FileType type)
{
    switch (type) {
    case FileType::JPG:
        return "photo";
        break;
    case FileType::MP3:
        return "audio";
        break;
    case FileType::MP4:
        return "video";
        break;
    default:
        return "";
        break;
    }
}

/*
void RestInterface::sendPut()
{
    if (cookies.isEmpty()){
        restConnector->sendRequest("http://134.0.116.177/accounts/login/",onSuccessToken,onFail,cookies,RestConnector::Type::GET,QVariantMap());
    } else
    {
        QVariantMap params =
        {
            {"power","61"},
            {"download","61"}
        };
        sendPut(params);
    }
}
*/

void RestInterface::put_hardware()
{
    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    auto generatePart = [](QString name, QByteArray value) -> QHttpPart
    {
        QHttpPart part;
        part.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\""+name+"\""));
        part.setBody(value);
        return part;
    };
    QList<QHttpPart> form;
    form.append(generatePart("power","77"));
    form.append(generatePart("download","99"));
    for (auto part : form)
    {
        multiPart->append(part);
    }
    RestConnector::getRestConnector()->sendRequest("http://134.0.116.177/api/registrars/00000000/",&RestInterface::onSuccessToken,&RestInterface::onFail,cookies,RestConnector::Type::PUT,QVariantMap(), multiPart);
}
