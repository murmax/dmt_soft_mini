#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QNetworkReply>
#include <QNetworkCookie>

QByteArray MainWindow::tokenTemp;
Token MainWindow::mainToken;
QList<QNetworkCookie> MainWindow::cookies;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    restConnector = RestConnector::getRestConnector();
	restConnector->initRequester("cloud-api.yandex.net",new QSslConfiguration());
//    sessionId.setName("sessionid");
//    sessionId.setValue("be6qmjkkhksrdhqca18tbocifid0o6or");
//    cookies.append(sessionId);

//    csrfToken.setName("csrftoken");
//    csrfToken.setValue("gnzuuOtxRSg96ANpkmLFaKhzMXhsqYamsz0nG5Z5OkyA6jWGFIp1NAuJQ2LZn02R");
//    cookies.append(csrfToken);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onSuccess( QNetworkReply *reply)
{
    qDebug()<<__func__<<reply->readAll();
}

void MainWindow::onSuccessToken( QNetworkReply *reply)
{
    QString res = reply->readAll();
    int len1 = res.indexOf("csrfmiddlewaretoken")+QString("csrfmiddlewaretoken").length();
    int len2 = res.indexOf("value",len1)+QString("value").length();
    int len3 = res.indexOf("\"",len2)+QString("\"").length();
    int len4 = res.indexOf("\"",len3);
    //QString apiKey = res.mid(len3,len4-len3);Form item: "csrfmiddlewaretoken" = "5reU6Eq5ZkDT0B11eAIRWTfV2lkF3aELeYQZUVTCnrKfcNq0K9JQ7ahVIjk1Yaec"
    tokenTemp = res.mid(len3,len4-len3).toUtf8();
    qDebug()<<"csrfmiddlewaretoken:"<<tokenTemp;

    if (!tokenTemp.isEmpty()){
        QVariantMap data;
        data.insert("csrfmiddlewaretoken",tokenTemp);
        data.insert("username","qwerty123@gmail.com ");
        data.insert("password","1234");
        RestConnector::getRestConnector()->sendRequest
                ("http://134.0.116.177/accounts/login/",onSuccessToken2,onFailToken2,cookies,RestConnector::Type::POST,data);
    }
}

void MainWindow::onSuccessToken2( QNetworkReply *reply)
{
    QList<QNetworkCookie> cookies1 = reply->header(QNetworkRequest::SetCookieHeader).value<QList<QNetworkCookie>>();
    cookies = cookies1;
    qDebug()<<Qt::endl<<"result success:"<<reply->readAll();
    for (auto e : cookies1)
    {
        qDebug()<<"Cookies:"<<e;
    }
    QVariantMap params =
    {
        {"power","55"},
        {"download","44"}
    };
    sendPut(params);


}

void MainWindow::onSuccessToken3(QNetworkReply *reply)
{
    qDebug()<<"result success:"<<reply->readAll();
}

void MainWindow::onFailToken2( QNetworkReply *reply)
{
    qDebug()<<"result fail:"<<reply->readAll();
}


void MainWindow::sendPut(QVariantMap params)
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
    for (auto iter = params.begin();iter!=params.end();iter++)
    {
        form.append(generatePart(iter.key(),iter.value().toByteArray()));
    }

    for (auto part : form)
    {
        multiPart->append(part);
    }
    RestConnector::getRestConnector()->sendRequest
            ("http://134.0.116.177/api/registrars/00000000/",onSuccessToken3,onFail,cookies,RestConnector::Type::PUT,QVariantMap(), multiPart);

}

void MainWindow::onFail( QNetworkReply *reply)
{
    qDebug()<<__func__<<reply->readAll();
    cookies.clear();
}

void MainWindow::on_btn_post_clicked()
{
    /*
    QVariantMap data;
    data.insert("registrar","00000000");
    data.insert("source","file:///C:/Users/Dmitry/Desktop/123.jpg");
    data.insert("name","123.jpg");
    data.insert("size","12345");
    data.insert("type","photo");
    data.insert("record_date","2018-07-22");
    data.insert("unload_date","2018-07-22");
    data.insert("owner","Test_max");
    */
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
    imagePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"source\"; filename=\"image.jpeg\""));
    QFile *file = new QFile("C:\\Users\\Dmitry\\Desktop\\123.jpg");
    file->open(QIODevice::ReadOnly);
    imagePart.setBodyDevice(file);
    file->setParent(multiPart);
    form.append(imagePart);

    form.append(generatePart("name","test.txt"));
    form.append(generatePart("size","887766"));
    form.append(generatePart("type","photo"));
    form.append(generatePart("record_date","2018-07-22"));
    form.append(generatePart("unload_date","2018-07-22"));
    form.append(generatePart("owner","Test_max"));

    for (const auto &part : form)
    {
        multiPart->append(part);
    }
    restConnector->sendRequest("http://134.0.116.177/api/files/",onSuccess,onFail,cookies,RestConnector::Type::POST_MULTI,QVariantMap(),multiPart);

}


void MainWindow::on_btn_get_clicked()
{
    if (cookies.isEmpty()){
        restConnector->sendRequest("http://134.0.116.177/accounts/login/",onSuccessToken,onFail,cookies,RestConnector::Type::GET,QVariantMap());
    } else
    {
        QVariantMap params =
        {
            {"power","66"},
            {"download","77"}
        };
        sendPut(params);
    }
}


void MainWindow::on_put_hardware_clicked()
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
    restConnector->sendRequest("http://134.0.116.177/api/registrars/00000000/",onSuccessToken,onFail,cookies,RestConnector::Type::PUT,QVariantMap(), multiPart);
}

