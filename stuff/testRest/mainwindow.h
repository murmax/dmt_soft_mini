#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <restconnector.h>
#include <QTextDocument>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
     static void onFail( QNetworkReply *obj);
     static void onSuccess( QNetworkReply *obj);
     static void onSuccessToken( QNetworkReply *obj);
     static void onSuccessToken2( QNetworkReply *obj);
     static void onSuccessToken3(QNetworkReply *reply);
     static void onFailToken2( QNetworkReply *obj);


     static void sendPut(QVariantMap params );

private slots:
    void on_btn_post_clicked();

    void on_btn_get_clicked();

    void on_put_hardware_clicked();

private:
    static QList<QNetworkCookie> cookies;
    QNetworkCookie csrfToken;
    QNetworkCookie sessionId;
    RestConnector* restConnector;
    Ui::MainWindow *ui;


    static QByteArray tokenTemp;
    static Token mainToken;
};
#endif // MAINWINDOW_H
