#include "http_mgr.h"

http_mgr::~http_mgr()
{

}


http_mgr::http_mgr() {
    connect(this,&http_mgr::sig_http_finish,this,&http_mgr::slot_http_finish);
}

void http_mgr::post_httpreq(QUrl url, QJsonObject json, ReqId req_id, Modules mod)
{
    //创建请求
    QByteArray data = QJsonDocument(json).toJson();
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader,QByteArray::number(data.length()));

    //发送请求，处理响应
    auto self = shared_from_this();
    QNetworkReply * reply = manager.post(request,data);

    connect(reply,&QNetworkReply::finished,[self,reply,req_id,mod](){
        //处理错误情况
        if(reply->error()!=QNetworkReply::NoError){
            qDebug()<<reply->errorString();
            emit self->sig_http_finish(req_id,"",ErrorCodes::ERR_NETWORK,mod);
            reply->deleteLater();
            return;
        }
        //无错误
        QString res=reply->readAll();

        //发送信号通知完成
        emit self->sig_http_finish(req_id,res,ErrorCodes::SUCCESS,mod);
        reply->deleteLater();
        return ;
    });
}

void http_mgr::slot_http_finish(ReqId id, QString res, ErrorCodes err, Modules mod)
{
    if(mod==Modules::REGISTERMOD){
        //发送信号通知指定模块http的响应结束了
        emit sig_reg_mod_finish(id,res,err);
    }
    if(mod==Modules::RESETMOD){
        //发送信号通知指定模块http的响应结束了
        emit sig_reset_mod_finish(id,res,err);
    }

    if(mod==Modules::LOGINMOD){
        emit sig_login_mod_finish(id,res,err);
    }
}
