#ifndef HTTP_MGR_H
#define HTTP_MGR_H
#include"singleton.h"
#include<QString>
#include<QUrl>
#include<QObject>
#include<QNetworkAccessManager>
#include<QJsonObject>
#include<QJsonDocument>

//CRTP
class http_mgr:public QObject,public singleton<http_mgr>,public std::enable_shared_from_this<http_mgr>
{
    Q_OBJECT

private:
    friend class singleton<http_mgr>;
    http_mgr();
    QNetworkAccessManager manager;


public:
    ~http_mgr();
    void post_httpreq(QUrl url,QJsonObject json,ReqId req_id,Modules mod); //发送http的post请求
private slots:
    void slot_http_finish(ReqId id , QString res,ErrorCodes err,Modules mod);
signals:
    void sig_http_finish(ReqId id , QString res,ErrorCodes err,Modules mod);//发送信号
    void sig_reg_mod_finish(ReqId id , QString res,ErrorCodes err);
    void sig_reset_mod_finish(ReqId id , QString res,ErrorCodes err);
    void sig_login_mod_finish(ReqId id , QString res,ErrorCodes err);

};

#endif // HTTP_MGR_H
