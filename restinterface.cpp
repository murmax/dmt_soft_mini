#include "restinterface.h"
#include <QNetworkReply>
#include <QNetworkCookie>
#include <QTimer>
#include <QFileInfo>

RestInterface* RestInterface::restInterfacePtr = nullptr;

RestInterface::RestInterface() : QObject(nullptr)
{
    restInterfacePtr = this;
    restConnector = RestConnector::getRestConnector();
    restConnector->initRequester(new QSslConfiguration());
}

void RestInterface::onSuccessPostFile( QNetworkReply *reply)
{
    emit fileSentSuccess(filesPosted.take(reply));
    qDebug()<<__func__<<reply->readAll();
}

void RestInterface::onFailPostFile(QNetworkReply *reply)
{
    emit fileSentFail(filesPosted.take(reply));
    qDebug()<<__func__<<reply->readAll();
}

void RestInterface::onSuccessToken( QNetworkReply *reply)
{
    QString res = reply->readAll();
    int len1 = res.indexOf("csrfmiddlewaretoken")+QString("csrfmiddlewaretoken").length();
    int len2 = res.indexOf("value",len1)+QString("value").length();
    int len3 = res.indexOf("\"",len2)+QString("\"").length();
    int len4 = res.indexOf("\"",len3);
    auto tokenTemp = res.mid(len3,len4-len3).toUtf8();

    if (!tokenTemp.isEmpty()){
        QVariantMap data;
        data.insert("csrfmiddlewaretoken",tokenTemp);
        data.insert("username",qUncompress(login));
        data.insert("password",qUncompress(password));
        RestConnector::getRestConnector()->sendRequest(
                    generateAddress(DMT_API::API_Login),
                    &RestInterface::onSuccessToken2,
                    &RestInterface::onFail,
                    cookies,
                    RestConnector::Type::POST,
                    data);
    }
}

void RestInterface::onSuccessToken2( QNetworkReply *reply)
{
    QList<QNetworkCookie> cookies1 = reply->header(QNetworkRequest::SetCookieHeader).value<QList<QNetworkCookie>>();
    cookies = cookies1;
    qDebug()<<__func__<<reply->readAll();
    for (const auto &e : qAsConst(cookies1))
    {
        qDebug()<<"Cookies:"<<e;
    }
    emit cookiesAreReady();
}

void RestInterface::onSuccessPutDev(QNetworkReply *reply)
{
    qDebug()<<__func__<<reply->readAll();
}

void RestInterface::sendPutDevParams(QVariantMap params, QString dev_serial_number)
{
    if (cookies.isEmpty()){
        QEventLoop loop;
        connect(this, &RestInterface::cookiesAreReady, &loop, &QEventLoop::quit);
        if (!cookiesAreBeingUpdated){
            cookiesAreBeingUpdated = true;
            restConnector->sendRequest(
                        generateAddress(DMT_API::API_Login),
                        &RestInterface::onSuccessToken,
                        &RestInterface::onFail,
                        cookies,
                        RestConnector::Type::GET,
                        QVariantMap());
        }
        loop.exec();
        cookiesAreBeingUpdated = false;
    }
    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QList<QHttpPart> form;
    for (auto iter = params.begin();iter!=params.end();iter++)
    {
        form.append(generatePartForQHttpMultiPart(iter.key(),iter.value().toByteArray()));
    }

    for (const auto &part : form)
    {
        multiPart->append(part);
    }
    RestConnector::getRestConnector()->sendRequest
            (generateAddress(DMT_API::API_Devs)+dev_serial_number+"/",
             &RestInterface::onSuccessPutDev,
             &RestInterface::onFail,
             cookies,
             RestConnector::Type::PUT,
             QVariantMap(),
             multiPart);
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
    //TODO: проанализировать ответ
    qDebug()<<__func__<<reply->readAll();
    cookies.clear();
}

void RestInterface::sendPostFile(DB_FILE_INFO* fileInfo)
{
    if(fileInfo != nullptr){

        QUrl urlFile(fileInfo->local_url);
        QFile file(urlFile.toLocalFile());
        if (file.open(QIODevice::ReadOnly)){
            QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

            QList<QHttpPart> form;
            form.append(generatePartForQHttpMultiPart("registrar",fileInfo->dev_serial_number.toLatin1()));

            QHttpPart imagePart;
            imagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/jpeg"));
            QString filename(QFileInfo(file).fileName());
            imagePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                                QString("form-data; name=\"source\"; filename=\"%1\"")
                                .arg(filename));
            imagePart.setBody(file.readAll());
            file.setParent(multiPart);
            form.append(imagePart);

            QPair<QString, FileType> pair = getNameAndTypeFromPath(fileInfo->local_url);

            form.append(generatePartForQHttpMultiPart("name",pair.first.toLatin1()));
            form.append(generatePartForQHttpMultiPart("size",fileInfo->size.toLatin1()));
            form.append(generatePartForQHttpMultiPart("type",fileTypeToString(pair.second).toLatin1()));
            form.append(generatePartForQHttpMultiPart("record_date",fileInfo->record_time.toLatin1()));
            form.append(generatePartForQHttpMultiPart("unload_date",fileInfo->download_time.toLatin1()));
            form.append(generatePartForQHttpMultiPart("owner","Test_max")); //TODO: откуда брать?

            for (const auto &part : form)
            {
                multiPart->append(part);
            }
            QNetworkReply* reply = RestConnector::getRestConnector()->sendRequest(
                        generateAddress(DMT_API::API_Files),
                        &RestInterface::onSuccessPostFile,
                        &RestInterface::onFailPostFile,
                        cookies,
                        RestConnector::Type::POST_MULTI,
                        QVariantMap(),
                        multiPart);
            filesPosted.insert(reply,fileInfo->local_url);
        }
    }
}

void RestInterface::sendPutDev(DevInfo *devInfo)
{
    QVariantMap params;
    params.insert("power",devInfo->devInfo.battery_lvl);
    params.insert("download",devInfo->devInfo.download_lvl);
    sendPutDevParams(params,devInfo->dev_serial_number);
}

QPair<QString, RestInterface::FileType> RestInterface::getNameAndTypeFromPath(QString filePath)
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

QHttpPart RestInterface::generatePartForQHttpMultiPart(QString name, QByteArray value)
{
    QHttpPart part;
    part.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\""+name+"\""));
    part.setBody(value);
    return part;
}

QString RestInterface::generateAddress(DMT_API apiType)
{
    return host+apiAddress.value(apiType);
}

